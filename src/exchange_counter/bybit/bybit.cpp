//
// Created by Bin Xu on 2021/6/5.
//
#include <iostream>
#include <vector>

#include <simdjson.h>

#include <websocket_data.h>

#include "bybit.h"

#include "parse_json.h"

using JsonParser = simdjson::ondemand::parser;
using JsonIter = simdjson::ondemand::document;
using JsonObj = simdjson::ondemand::object;
using JsonArray = simdjson::ondemand::array;

using namespace std;

namespace exchange_counter {
namespace bybit {

class BybitImpl {

	using OnOrderBookUpdate = Bybit::OnOrderBookUpdate;
	using OnTradeUpdate = Bybit::OnTradeUpdate;

	vector<string> subscription_topics_;
	OnOrderBookUpdate callback__orderbook_XRPUSD_;
	OnTradeUpdate callback__trade_XRPUSD_;

	OrderBook data__orderbook_XRPUSD_;

	JsonParser parser_;

	unique_ptr<WsData> ws_data_;

	bool process_msg(const char* msg_data, std::size_t msg_len, std::size_t buf_len) {
		cout<<"msg_len="<<msg_len<<" buf_len="<<buf_len<<"\n\n";

		WhichOne which_one;
		Trade trade_xrpusd;

		JsonIter iter = parser_.iterate(msg_data, msg_len, buf_len);

		parse_json(move(iter), which_one, data__orderbook_XRPUSD_, trade_xrpusd);

		switch (which_one) {
			case NOT_INTERESTED: {
				cout << "got NOT_INTERESTED msg\n\n";
				break;
			}
			case ORDER_BOOK_XRPUSD: {
				cout << "got ORDER_BOOK_XRPUSD:\n";
				cout << data__orderbook_XRPUSD_.str() <<"\n\n";
				return callback__orderbook_XRPUSD_(data__orderbook_XRPUSD_);
				break;
			}
			case TRADE_XRPUSD: {
				cout << "got TRADE_XRPUSD:\n";
				cout << trade_xrpusd.str() <<"\n\n";
				return callback__trade_XRPUSD_(trade_xrpusd);
				break;
			}
		}

		return true;
	}

public:
	BybitImpl() {
	}

	void subscribe_XRPUSD_orderbook(OnOrderBookUpdate update_func) {
		subscription_topics_.emplace_back(R"({"op": "subscribe", "args": ["orderBookL2_25.XRPUSD"]})");
		callback__orderbook_XRPUSD_ = move(update_func);
	}

	void subscribe_XRPUSD_trade(OnTradeUpdate update_func) {
		subscription_topics_.emplace_back(R"({"op":"subscribe","args":["trade.XRPUSD"]})");
		callback__trade_XRPUSD_ = move(update_func);
	}

	void subscribe_done(boost::asio::io_context &ioc) {
		ws_data_ = make_unique<WsData>("stream.bytick.com", "/realtime", "443",
								 5, R"({"op":"ping"})",
								 subscription_topics_,
								 [this](const char* msg_data, std::size_t msg_len, std::size_t buf_len)
								 {
									return process_msg(msg_data, msg_len, buf_len);
								 },
								 ioc);
	}
};


Bybit::Bybit() {
	impl_ = make_unique<BybitImpl>();
}

Bybit::~Bybit() = default;

void Bybit::subscribe_XRPUSD_orderbook(OnOrderBookUpdate update) {
	impl_->subscribe_XRPUSD_orderbook(move(update));
}

void Bybit::subscribe_XRPUSD_trade(OnTradeUpdate update) {
	impl_->subscribe_XRPUSD_trade(move(update));
}

void Bybit::subscribe_done(boost::asio::io_context &ioc) {
	impl_->subscribe_done(ioc);
}

}
}
