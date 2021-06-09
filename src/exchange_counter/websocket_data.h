//
// Created by Bin Xu on 2021/5/23.
//

#ifndef ALTUM_WEBSOCKET_DATA_H
#define ALTUM_WEBSOCKET_DATA_H

#include <memory>
#include <functional>

namespace boost {
namespace asio {
class io_context;
}
}

namespace exchange_counter {

class WsDataImpl;

class WsData {
public:
	using MsgProcessor = std::function<bool(const char*, std::size_t, std::size_t)>;

	WsData(const std::string &host, const std::string &endpoint, const std::string &port,
		   int seconds_wait_to_reset_connection, const std::string &ping_str,
		   std::vector<std::string> subscription_topics, MsgProcessor msg_processor,
		   boost::asio::io_context &ioc);
	~WsData();

private:
	std::unique_ptr<WsDataImpl> impl_;
};

}

#endif //ALTUM_WEBSOCKET_DATA_H
