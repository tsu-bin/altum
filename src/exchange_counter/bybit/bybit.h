//
// Created by Bin Xu on 2021/6/5.
//

#ifndef ALTUM_BYBIT_H
#define ALTUM_BYBIT_H

#include <memory>
#include <functional>

#include <market_data.h>

namespace boost {
namespace asio {
class io_context;
}
}


namespace exchange_counter {
namespace bybit {

class BybitImpl;

class Bybit {
public:
	Bybit();
	~Bybit();

	using OnOrderBookUpdate = std::function<bool(const OrderBook&)>;
	using OnTradeUpdate = std::function<bool(const Trade&)>;

	void subscribe_XRPUSD_orderbook(OnOrderBookUpdate update_func);
	void subscribe_XRPUSD_trade(OnTradeUpdate update_func);

	void subscribe_done(boost::asio::io_context &ioc);

private:
	std::unique_ptr<BybitImpl> impl_;
};

}
}

#endif //ALTUM_BYBIT_H
