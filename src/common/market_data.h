//
// Created by Bin Xu on 2021/5/23.
//

#ifndef ALTUM_MARKETDATA_H
#define ALTUM_MARKETDATA_H


#include <map>
#include <string>

using PriceOfTick = int64_t;
const PriceOfTick k_price_of_tick_max_init = std::numeric_limits<PriceOfTick>::max();
const PriceOfTick k_price_of_tick_abnormal_max_threshold = k_price_of_tick_max_init / 2;
using Size = int64_t;
using Timestamp = int64_t;

enum Side {
	ASK = 0,
	BID = 1
};

class OrderBook {
public:
	PriceOfTick spread() const {
		return ask_levels_.begin()->first - bid_levels_.rbegin()->first;
	}
	PriceOfTick mid_price() const {
		return (ask_levels_.begin()->first + bid_levels_.rbegin()->first)/2;
	}

	void insert_or_update_ask_level(PriceOfTick price, Size size) {
		ask_levels_[price] = size;
	}
	void insert_or_update_bid_level(PriceOfTick price, Size size) {
		bid_levels_[price] = size;
	}

	void del_ask_level(PriceOfTick price) {
		ask_levels_.erase(ask_levels_.find(price));
	}
	void del_bid_level(PriceOfTick price) {
		bid_levels_.erase(ask_levels_.find(price));
	}

	void clear() {
		ask_levels_.clear();
		bid_levels_.clear();
	}

	Timestamp timestamp;

	std::string str() const {
		std::string ret;

		return ret;
	}
private:
	std::map<PriceOfTick, Size> ask_levels_{{k_price_of_tick_max_init, 0}};
	std::map<PriceOfTick, Size> bid_levels_{{0, 0}};
};

struct Trade {
public:
	PriceOfTick price;
	Size size;
	Side side;
	Timestamp timestamp;

	std::string str() const {
		std::string ret;

		return ret;
	}
};


#endif //ALTUM_MARKETDATA_H
