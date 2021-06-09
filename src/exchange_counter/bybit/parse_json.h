//
// Created by Bin Xu on 2021/6/9.
//

#ifndef ALTUM_PARSE_JSON_H
#define ALTUM_PARSE_JSON_H

#include <simdjson.h>

#include "bybit.h"

using JsonParser = simdjson::ondemand::parser;
using JsonIter = simdjson::ondemand::document;
using JsonObj = simdjson::ondemand::object;
using JsonArray = simdjson::ondemand::array;

using namespace std;

enum WhichOne {
	NOT_INTERESTED,
	ORDER_BOOK_XRPUSD,
	TRADE_XRPUSD,
};

bool parse_json(JsonIter& it, WhichOne& which, OrderBook& od_xrpusd, vector<Trade>& trades_xrpusd);

#endif //ALTUM_PARSE_JSON_H
