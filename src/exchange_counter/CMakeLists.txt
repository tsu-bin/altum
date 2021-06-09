
add_library(_websocket_data
        websocket_data.cpp)

if(DEFINED USE_VALGRIND)
    target_compile_definitions(_websocket_data PRIVATE BOOST_USE_VALGRIND=x)
endif(DEFINED USE_VALGRIND)

target_include_directories(_websocket_data
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(_websocket_data
        util
        ext-boost pthread ${OPENSSL_LIBRARIES}
        )

add_subdirectory(bybit)
