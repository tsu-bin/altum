//
// Created by Bin Xu on 2021/6/9.
//

#include <sstream>

#include "market_data.h"

using namespace std;

string OrderBook::str() const {
	std::stringstream ss;
	ss<<"order_book "<<symbol<<": mid_price="<<mid_price()<<" spread="<<spread()<<"\n";
	ss<<"ask:\tprice\t\tsize\n";
	for(auto it=ask_levels_.rbegin(); it!=ask_levels_.rend(); ++it){
		ss<<"\t"<<it->first<<"\t\t"<<it->second<<"\n";
	}
	ss<<"bid:\tprice\t\tsize\n";
	for(auto it=bid_levels_.begin(); it!=bid_levels_.end(); ++it){
		ss<<"\t"<<it->first<<"\t\t"<<it->second<<"\n";
	}
	return ss.str();
}

string Trade::str() const {
	std::stringstream ss;
	ss<<"trade "<<symbol<<": price="<<price<<" size="<<size;
	return ss.str();
}