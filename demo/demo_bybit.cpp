//
// Created by Bin Xu on 2021/5/23.
//
#include <iostream>

#include <boost/asio.hpp>
#include <bybit.h>

using namespace std;

int main(int argc, char** argv)
{
	boost::asio::io_context ioc;

	exchange_counter::bybit::Bybit bybit{};

//	bybit.subscribe_XRPUSD_orderbook(
//			[](const OrderBook& od){
//				cout<<"got XRPUSD orderbook, mid-price="<<od.mid_price()<<" spread="<<od.spread()<<"\n\n";
//				return true;
//			});

	bybit.subscribe_XRPUSD_trade(
			[](const Trade& trade){
				cout<<"got XRPUSD trade, price="<<trade.price<<" size="<<trade.size<<"\n\n";
				return true;
			});

	bybit.subscribe_done(ioc);

	ioc.run();

	return EXIT_SUCCESS;
}