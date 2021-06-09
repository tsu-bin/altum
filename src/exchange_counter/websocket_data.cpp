#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/io_context_strand.hpp>
#include <boost/asio/system_timer.hpp>
#include <boost/asio/spawn.hpp>

#include "websocket_data.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

using namespace std;

namespace exchange_counter {

class WsDataImpl {
public:
	WsDataImpl(const string &host, const string &endpoint, const string &port,
		   int seconds_wait_to_reset_connection, const string &ping_str,
		   vector<string> subscription_topics, WsData::MsgProcessor msg_processor,
		   net::io_context &ioc) :
		   		host_{host}, endpoint_{endpoint}, port_{port},
				duration_to_check_liveness_{std::chrono::seconds(seconds_wait_to_reset_connection)}, ping_str_{ping_str},
				subscription_topics_(move(subscription_topics)), msg_processor_(move(msg_processor)),

				ssl_ctx_{ssl::context::tls_client}, //ssl_ctx_{ssl::context::tlsv12_client},
				strand_{net::make_strand(ioc.get_executor())}
	{
		ssl_ctx_.set_default_verify_paths();

		//buffer_.reserve(k_buffer_initial_size_);

		net::spawn(strand_,
				   [this](boost::asio::yield_context yield) {
					   coro_do_work(yield);
				   });
	}

private:
	using Websocket = websocket::stream<beast::ssl_stream<beast::tcp_stream>>;

	const std::string host_;
	const std::string endpoint_;
	const std::string port_;

	const std::chrono::seconds duration_to_check_liveness_;
	const std::string ping_str_;

	const size_t k_buffer_initial_size_ = 2<<20;

	const vector<string> subscription_topics_;
	WsData::MsgProcessor msg_processor_;

	ssl::context ssl_ctx_;

	net::strand<net::io_context::executor_type> strand_;

	struct WebsocketWithVersionNumber {
		Websocket ws;
		const int ver;

		WebsocketWithVersionNumber(Websocket &&_ws, int _ver) : ws(std::move(_ws)), ver(_ver) {
		}
		~WebsocketWithVersionNumber() {
			beast::error_code ec;
			ws.close(websocket::close_code::normal, ec);
			if (ec) {
				cout << "websocket close failed, detail: " << ec.message() << endl;
			}
		}
	};

	[[noreturn]] void coro_do_work(net::yield_context &yield) {
		beast::error_code ec;
		auto last_data_recieved_timepoint = std::chrono::system_clock::now();

		tcp::resolver resolver(strand_);

		std::unique_ptr<WebsocketWithVersionNumber> ws_v;
		int ver = 0;
		RECONNECT:
		ws_v = std::make_unique<WebsocketWithVersionNumber>(Websocket{strand_, ssl_ctx_}, ver++);
		cout << "new websocket built\n";

		auto const results = resolver.async_resolve(host_, port_, yield[ec]);
		if (ec) {
			cout << "resolve failed, detail: " << ec.message() << endl;
			goto RECONNECT;
		}

		auto &ws = ws_v->ws;
		//beast::get_lowest_layer(ws).expires_after(std::chrono::seconds(30));
		beast::get_lowest_layer(ws).async_connect(results, yield[ec]);
		if (ec) {
			cout << "connect failed, detail: " << ec.message() << endl;
			goto RECONNECT;
		}

		beast::get_lowest_layer(ws).expires_after(std::chrono::seconds(30));
		// Set a decorator to change the User-Agent of the handshake
//		ws.set_option(websocket::stream_base::decorator(
//				[](websocket::request_type& req) {
//					req.set(http::field::user_agent, "my_customerised_ua");
//				}));
		SSL_set_tlsext_host_name(ws.next_layer().native_handle(), host_.c_str());
		ws.next_layer().async_handshake(ssl::stream_base::client, yield[ec]);
		if (ec) {
			cout << "ssl_handshake failed, detail: " << ec.message() << endl;
			goto RECONNECT;
		}

		beast::get_lowest_layer(ws).expires_never();
		// Set suggested timeout settings for the websocket
		ws.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));
		// Perform the websocket handshake
		ws.async_handshake(host_, endpoint_, yield[ec]);
		if (ec) {
			cout << "websocket_handshake failed, detail: " << ec.message() << endl;
			goto RECONNECT;
		}

		for (auto& topic : subscription_topics_) {
			ws.async_write(net::buffer(topic), yield[ec]);
			if (ec) {
				cout << "websocket write subscription_topic failed, detail: " << ec.message() << endl;
				goto RECONNECT;
			}
		}

		net::spawn(strand_,
				   [this, &ws_v, ver = ws_v->ver, &last_data_recieved_timepoint](boost::asio::yield_context yield) {
					   coro_monitor(ws_v, ver, last_data_recieved_timepoint, yield);
				   });
		cout << "!!! coro_monitor spawned, ver = " << ws_v->ver << "\n\n";

		//buffer_.clear();
		beast::flat_buffer buffer_;
		buffer_.reserve(k_buffer_initial_size_);
		while (true) {
			ws.async_read(buffer_, yield[ec]);
			if (ec) {
				cout << "websocket read failed, detail: " << ec.message() << endl;
				goto RECONNECT;
			}
			last_data_recieved_timepoint = std::chrono::system_clock::now();

			//cout << "got data:\n"; cout << beast::make_printable(buffer_.data()) << "\n\n";
			if (not msg_processor_((const char*)buffer_.data().data(), buffer_.data().size(), buffer_.capacity())) {
				cout << "msg_processor_ failed" << endl;
				goto RECONNECT;
			}
			buffer_.clear();
		}
	}

	void coro_monitor(std::unique_ptr<WebsocketWithVersionNumber> &ws_v, const int version_saved,
					  auto &last_data_recieved_timepoint, net::yield_context &yield) {

		boost::asio::system_timer timer(strand_);

		bool ping_sent = false;

		while (true) {
			beast::error_code ec;

			timer.expires_after(duration_to_check_liveness_);
			timer.async_wait(yield[ec]);
			if (ec) {
				cout << "timer.async_wait failed, detail: " << ec.message() << endl;
			}

			if ((not ws_v) || (ws_v->ver != version_saved)) {
				cout << "the websocket is reset, monitor coro quit now\n";
				cout << "!!! coro_monitor quit now, ver = " << version_saved << "\n\n";
				return;
			}

			auto now = std::chrono::system_clock::now();
			if ((now - last_data_recieved_timepoint) > duration_to_check_liveness_) {
				if (not ping_sent) {
					cout << "timeout! send ping to keep alive\n";
					ws_v->ws.async_write(net::buffer(ping_str_), yield[ec]);
					if (ec) {
						cout << "timeout, send ping failed, detail: " << ec.message() << endl;
					}
					ping_sent = true;
					continue;
				} else {
					cout << "still timeout after ping sent! going to close websocket\n";
					ws_v->ws.async_close(websocket::close_code::normal, yield[ec]);
					if (ec) {
						cout << "websocket async_close failed, detail: " << ec.message() << endl;
					}
					cout << "!!! coro_monitor quit now, ver = " << version_saved << "\n\n";

					return;
				}
			} else {
				ping_sent = false;
				cout << "no timeout, wait again\n";
			}
		}
	}
};


WsData::WsData(const std::string &host, const std::string &endpoint, const std::string &port,
			   int seconds_wait_to_reset_connection, const std::string &ping_str,
			   std::vector<std::string> subscription_topics, MsgProcessor msg_processor,
			   boost::asio::io_context &ioc) {
	impl_ = make_unique<WsDataImpl>(host, endpoint, port,
									seconds_wait_to_reset_connection, ping_str,
									move(subscription_topics), move(msg_processor),
									ioc);
}

WsData::~WsData() = default;

} //namespace exchange_counter
