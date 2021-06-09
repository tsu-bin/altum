//
// Created by Bin Xu on 2021/6/9.
//

#include "parse_json.h"

void parse_trade_array(JsonObj& obj_top_level, vector<Trade>& trades) {
	for(JsonObj obj : obj_top_level["data"]) {
		Trade trade;
		trade.price = obj["price"].get_double(k_XRPUSD_tick_precision);
		trade.size = obj["size"];
		trades.push_back(trade);
	}
}

void parse_orderbook_levels_update(JsonArray json_levels, OrderBook& od) {
	for (JsonObj json_level : json_levels) {
		PriceOfTick price = json_level["price"].get_double(k_XRPUSD_tick_precision);
		string_view side = json_level["side"];
		Size size = json_level["size"];

		if (side == "Buy") {
			od.insert_or_update_bid_level(price, size);
		}
		else {
			od.insert_or_update_ask_level(price, size);
		}
	}
}

void parse_orderbook_levels_del(JsonArray json_levels, OrderBook& od) {
	for (JsonObj json_level : json_levels) {
		PriceOfTick price = json_level["price"].get_double(k_XRPUSD_tick_precision);
		string_view side = json_level["side"];

		if (side == "Buy") {
			od.del_ask_level(price);
		}
		else {
			od.del_bid_level(price);
		}
	}
}

void parse_orderbook(JsonObj& obj_top_level, OrderBook& od) {
	if (obj_top_level["type"] == "delta") {
		parse_orderbook_levels_del(obj_top_level["data"]["delete"].get_array(), od);
		parse_orderbook_levels_update(obj_top_level["data"]["update"].get_array(), od);
		parse_orderbook_levels_update(obj_top_level["data"]["insert"].get_array(), od);
		return;
	}

	if (obj_top_level["type"] == "snapshot") {
		parse_orderbook_levels_update(obj_top_level["data"].get_array(), od);
		return;
	}
}

bool parse_json(JsonIter& it, WhichOne& which, OrderBook& od_xrpusd, vector<Trade>& trades_xrpusd) {

	JsonObj obj_top_level = it.get_object();
	auto find_topic = obj_top_level.find_field("topic");
	if (find_topic.error()) {
		which = WhichOne::NOT_INTERESTED;
		return true;
	}

	string_view topic = find_topic.get_string();
	//cout<<"topic="<<topic;

	if (topic == "trade.XRPUSD") {
		which = WhichOne::TRADE_XRPUSD;
		parse_trade_array(obj_top_level, trades_xrpusd);
		return true;
	}

	if (topic == "orderBookL2_25.XRPUSD") {
		which = WhichOne::ORDER_BOOK_XRPUSD;
		parse_orderbook(obj_top_level, od_xrpusd);
		return true;
	}

	return true;
}