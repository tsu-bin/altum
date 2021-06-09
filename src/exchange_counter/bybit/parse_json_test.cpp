//
// Created by Bin Xu on 2021/6/9.
//

#include "parse_json.h"

using namespace std;
using namespace simdjson;

int main(void) {

	ondemand::parser parser;

	auto json_not_interested = R"({"success":true,"ret_msg":"","conn_id":"2eae2350-a6ed-4667-bdb2-87db062ff260","request":{"op":"subscribe","args":["trade.XRPUSD"]}}
)"_padded;

	auto json_od_snapshot_xrpusd = R"({"topic":"orderBookL2_25.XRPUSD","type":"snapshot","data":[{"price":"0.8637","symbol":"XRPUSD","id":8637,"side":"Buy","size":125022},{"price":"0.8638","symbol":"XRPUSD","id":8638,"side":"Buy","size":128312},{"price":"0.8639","symbol":"XRPUSD","id":8639,"side":"Buy","size":127067},{"price":"0.8640","symbol":"XRPUSD","id":8640,"side":"Buy","size":49622},{"price":"0.8641","symbol":"XRPUSD","id":8641,"side":"Buy","size":83981},{"price":"0.8642","symbol":"XRPUSD","id":8642,"side":"Buy","size":268197},{"price":"0.8643","symbol":"XRPUSD","id":8643,"side":"Buy","size":27355},{"price":"0.8644","symbol":"XRPUSD","id":8644,"side":"Buy","size":61391},{"price":"0.8645","symbol":"XRPUSD","id":8645,"side":"Buy","size":73498},{"price":"0.8646","symbol":"XRPUSD","id":8646,"side":"Buy","size":47509},{"price":"0.8647","symbol":"XRPUSD","id":8647,"side":"Buy","size":43354},{"price":"0.8648","symbol":"XRPUSD","id":8648,"side":"Buy","size":50094},{"price":"0.8649","symbol":"XRPUSD","id":8649,"side":"Buy","size":44614},{"price":"0.8650","symbol":"XRPUSD","id":8650,"side":"Buy","size":76775},{"price":"0.8651","symbol":"XRPUSD","id":8651,"side":"Buy","size":187054},{"price":"0.8652","symbol":"XRPUSD","id":8652,"side":"Buy","size":55740},{"price":"0.8653","symbol":"XRPUSD","id":8653,"side":"Buy","size":70649},{"price":"0.8654","symbol":"XRPUSD","id":8654,"side":"Buy","size":120560},{"price":"0.8655","symbol":"XRPUSD","id":8655,"side":"Buy","size":96082},{"price":"0.8656","symbol":"XRPUSD","id":8656,"side":"Buy","size":106141},{"price":"0.8657","symbol":"XRPUSD","id":8657,"side":"Buy","size":90086},{"price":"0.8658","symbol":"XRPUSD","id":8658,"side":"Buy","size":148020},{"price":"0.8659","symbol":"XRPUSD","id":8659,"side":"Buy","size":108449},{"price":"0.8660","symbol":"XRPUSD","id":8660,"side":"Buy","size":85721},{"price":"0.8661","symbol":"XRPUSD","id":8661,"side":"Buy","size":1257696},{"price":"0.8662","symbol":"XRPUSD","id":8662,"side":"Sell","size":47465},{"price":"0.8663","symbol":"XRPUSD","id":8663,"side":"Sell","size":60672},{"price":"0.8664","symbol":"XRPUSD","id":8664,"side":"Sell","size":60587},{"price":"0.8665","symbol":"XRPUSD","id":8665,"side":"Sell","size":2329},{"price":"0.8666","symbol":"XRPUSD","id":8666,"side":"Sell","size":15593},{"price":"0.8667","symbol":"XRPUSD","id":8667,"side":"Sell","size":19805},{"price":"0.8668","symbol":"XRPUSD","id":8668,"side":"Sell","size":52755},{"price":"0.8669","symbol":"XRPUSD","id":8669,"side":"Sell","size":27104},{"price":"0.8670","symbol":"XRPUSD","id":8670,"side":"Sell","size":127211},{"price":"0.8671","symbol":"XRPUSD","id":8671,"side":"Sell","size":30847},{"price":"0.8672","symbol":"XRPUSD","id":8672,"side":"Sell","size":53035},{"price":"0.8673","symbol":"XRPUSD","id":8673,"side":"Sell","size":33754},{"price":"0.8674","symbol":"XRPUSD","id":8674,"side":"Sell","size":36516},{"price":"0.8675","symbol":"XRPUSD","id":8675,"side":"Sell","size":51048},{"price":"0.8676","symbol":"XRPUSD","id":8676,"side":"Sell","size":24876},{"price":"0.8677","symbol":"XRPUSD","id":8677,"side":"Sell","size":67748},{"price":"0.8678","symbol":"XRPUSD","id":8678,"side":"Sell","size":54439},{"price":"0.8679","symbol":"XRPUSD","id":8679,"side":"Sell","size":40378},{"price":"0.8680","symbol":"XRPUSD","id":8680,"side":"Sell","size":143940},{"price":"0.8681","symbol":"XRPUSD","id":8681,"side":"Sell","size":130119},{"price":"0.8682","symbol":"XRPUSD","id":8682,"side":"Sell","size":126390},{"price":"0.8683","symbol":"XRPUSD","id":8683,"side":"Sell","size":746770},{"price":"0.8684","symbol":"XRPUSD","id":8684,"side":"Sell","size":123928},{"price":"0.8685","symbol":"XRPUSD","id":8685,"side":"Sell","size":175770},{"price":"0.8686","symbol":"XRPUSD","id":8686,"side":"Sell","size":108396}],"cross_seq":2376917900,"timestamp_e6":1623154956544716})"_padded;
	auto json_od_update_xrpusd = R"({"topic":"orderBookL2_25.XRPUSD","type":"delta","data":{"delete":[],"update":[{"price":"0.8676","symbol":"XRPUSD","id":8676,"side":"Sell","size":32876},{"price":"0.8678","symbol":"XRPUSD","id":8678,"side":"Sell","size":46439}],"insert":[],"transactTimeE6":0},"cross_seq":2376917901,"timestamp_e6":1623154956643979})"_padded;

	auto json_trade_xrpusd = R"({"topic":"trade.XRPUSD","data":[{"trade_time_ms":1623154947808,"timestamp":"2021-06-08T12:22:27.000Z","symbol":"XRPUSD","side":"Buy","size":8,"price":0.8659,"tick_direction":"ZeroPlusTick","trade_id":"33d2d45d-1f3e-5294-8c00-912b6409a6aa","cross_seq":2376917119},{"trade_time_ms":1623154947808,"timestamp":"2021-06-08T12:22:27.000Z","symbol":"XRPUSD","side":"Buy","size":24,"price":0.8659,"tick_direction":"ZeroPlusTick","trade_id":"c4526980-a766-568a-9f44-42e1ef8ab890","cross_seq":2376917119},{"trade_time_ms":1623154947808,"timestamp":"2021-06-08T12:22:27.000Z","symbol":"XRPUSD","side":"Buy","size":1001,"price":0.8659,"tick_direction":"ZeroPlusTick","trade_id":"9e69ba26-e603-5a34-b005-eb60686dcd32","cross_seq":2376917119}]})"_padded;

	WhichOne which_one;
	OrderBook orderbook_xrpusd;
	Trade trade_xrpusd;

	auto print_result = [&](const string& what) {
		cout << what << ":\n";
		switch (which_one) {
			case NOT_INTERESTED: {
				cout << "got NOT_INTERESTED msg\n\n";
				break;
			}
			case ORDER_BOOK_XRPUSD: {
				cout << "got ORDER_BOOK_XRPUSD:\n";
				cout << orderbook_xrpusd.str() <<"\n\n";
				break;
			}
			case TRADE_XRPUSD: {
				cout << "got TRADE_XRPUSD:\n";
				cout << trade_xrpusd.str() <<"\n\n";
				break;
			}
		}
	};

	JsonIter iter = parser.iterate(json_not_interested);
	parse_json(move(iter), which_one, orderbook_xrpusd, trade_xrpusd);
	print_result("parse json_not_interested");

	iter = parser.iterate(json_od_snapshot_xrpusd);
	parse_json(move(iter), which_one, orderbook_xrpusd, trade_xrpusd);
	print_result("parse json_od_snapshot_xrpusd");

	iter = parser.iterate(json_od_update_xrpusd);
	parse_json(move(iter), which_one, orderbook_xrpusd, trade_xrpusd);
	print_result("parse json_od_update_xrpusd");

	iter = parser.iterate(json_trade_xrpusd);
	parse_json(move(iter), which_one, orderbook_xrpusd, trade_xrpusd);
	print_result("parse json_trade_xrpusd");
}