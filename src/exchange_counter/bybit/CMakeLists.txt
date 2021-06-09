add_library(bybit
        bybit.cpp parse_json.cpp)

target_include_directories(bybit
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(bybit
        util common
        simdjson
        _websocket_data
        )

add_executable(test_parse_json parse_json_test.cpp parse_json.cpp)

target_link_libraries(test_parse_json
        util common
        simdjson
        )