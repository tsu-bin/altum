
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/io_context_strand.hpp>
#include <boost/asio/system_timer.hpp>
#include <boost/asio/spawn.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

using namespace std;
//------------------------------------------------------------------------------

class WsData {
private:
	using Websocket = websocket::stream<beast::ssl_stream<beast::tcp_stream>>;

	const std::string host_;
	const std::string port_;

	std::chrono::seconds duration_to_check_liveness;

	ssl::context ssl_ctx_;

	net::strand<net::io_context::executor_type> strand_;

	const string ping_str_;

public:
	WsData(const std::string& host, const std::string& port, int seconds_wait_to_reset_connection, const string& ping_str, net::io_context& ioc):
			host_(host), port_(port), ssl_ctx_{ssl::context::tlsv12_client},
			duration_to_check_liveness(std::chrono::seconds(seconds_wait_to_reset_connection)),
			strand_(net::make_strand(ioc.get_executor())),
			ping_str_(ping_str){
		ssl_ctx_.set_default_verify_paths();
	}

	void coro_monitor(Websocket& ws, auto& last_data_recieved_timepoint, net::yield_context& yield) {

		boost::asio::system_timer timer(strand_);

		bool ping_sent = false;

		while(true) {
			beast::error_code ec;

			timer.expires_after(duration_to_check_liveness);
			timer.async_wait(yield[ec]);
			if(ec) {
				cout << "timer.async_wait, detail: " << ec.message() << endl;
			}

			auto now = std::chrono::system_clock::now();
			if ((now - last_data_recieved_timepoint) > duration_to_check_liveness) {
				if (not ping_sent) {
					cout << "timeout! send ping to keep alive\n";
					ws.async_write(net::buffer(ping_str_), yield[ec]);
					if(ec) {
						cout << "timeout, send ping failed, detail: " << ec.message() << endl;
					}
					ping_sent = true;
					continue;
				}
				else {
					cout << "still timeout after ping sent! going to close websocket\n";
					ws.async_close(websocket::close_code::normal, yield[ec]);
					if(ec) {
						cout << "websocket close failed, detail: " << ec.message() << endl;
					}
					return;
				}
			}
			else {
				ping_sent = false;
				cout << "no timeout, wait again\n";
			}
		}

	}

	[[noreturn]] void coro_do_work(net::yield_context& yield) {
		beast::error_code ec;
		auto last_data_recieved_timepoint = std::chrono::system_clock::now();

		tcp::resolver resolver(strand_);

		std::unique_ptr<Websocket> ws;
RECONNECT:
		ws = std::make_unique<Websocket>(strand_, ssl_ctx_);
		cout << "new websocket built\n";

		auto const results = resolver.async_resolve(host_, port_, yield[ec]);
		if(ec) {
			cout << "resolve failed, detail: " << ec.message() << endl;
			goto RECONNECT;
		}

		beast::get_lowest_layer(*ws).expires_after(std::chrono::seconds(30));
		beast::get_lowest_layer(*ws).async_connect(results, yield[ec]);
		if(ec) {
			cout << "connect failed, detail: " << ec.message() << endl;
			goto RECONNECT;
		}

		beast::get_lowest_layer(*ws).expires_after(std::chrono::seconds(30));
		// Set a decorator to change the User-Agent of the handshake
		ws->set_option(websocket::stream_base::decorator(
				[](websocket::request_type& req) {
					req.set(http::field::user_agent, "my_customerised_ua");
				}));
		ws->next_layer().async_handshake(ssl::stream_base::client, yield[ec]);
		if(ec) {
			cout << "ssl_handshake failed, detail: " << ec.message() << endl;
			goto RECONNECT;
		}

		beast::get_lowest_layer(*ws).expires_never();
		// Set suggested timeout settings for the websocket
		ws->set_option( websocket::stream_base::timeout::suggested(beast::role_type::client));
		// Perform the websocket handshake
		ws->async_handshake(host_, "/", yield[ec]);
		if(ec) {
			cout << "websocket_handshake failed, detail: " << ec.message() << endl;
			goto RECONNECT;
		}

		//string msg_send = "{\"op\": \"subscribe\", \"args\": [\"trade:ADAUSDT\"]}";
		string msg_send = "ping, please echo pong";
		ws->async_write(net::buffer(msg_send), yield[ec]);
		if(ec) {
			cout << "websocket write failed, detail: " << ec.message() << endl;
			goto RECONNECT;
		}

		net::spawn(strand_,
				   [this, &ws = *ws, &last_data_recieved_timepoint](boost::asio::yield_context yield){
					   coro_monitor(ws, last_data_recieved_timepoint, yield);
				   });

		beast::flat_buffer buffer;
		while(true) {
			ws->async_read(buffer, yield[ec]);
			if(ec) {
				cout << "websocket read failed, detail: " << ec.message() << endl;
				goto RECONNECT;
			}
			last_data_recieved_timepoint = std::chrono::system_clock::now();

			cout << "got data:\n";
			cout << beast::make_printable(buffer.data()) << std::endl;
			buffer.clear();
		}
	}

	void submit_coro() {
		net::spawn(strand_,
					 [this](boost::asio::yield_context yield){
						 coro_do_work(yield);
					});
	}
};

//------------------------------------------------------------------------------

template<class T>
class ShowType; //ShowType<decltype(x)>{};

int main(int argc, char** argv)
{
    if(argc != 3)
    {
        std::cerr <<
            "Example:\n" <<
            " ./src/ExchangeCounter/exchange_counter echo.websocket.org 443'\n";
			" valgrind --leak-check=full ./src/ExchangeCounter/exchange_counter echo.websocket.org 443'\n";
        return EXIT_FAILURE;
    }
    auto const host = argv[1];
    auto const port = argv[2];

	net::io_context ioc;

	WsData ws_data(host, port, 5, "ping_to_keep_alive", ioc);
	ws_data.submit_coro();


    ioc.run();

    return EXIT_SUCCESS;
}