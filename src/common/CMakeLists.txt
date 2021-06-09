add_library(common
        market_data.cpp)
target_include_directories(common
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})