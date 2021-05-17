
add_executable(exchange_counter WsData.cpp)
if(DEFINED USE_VALGRIND)
    target_compile_definitions(exchange_counter PRIVATE BOOST_USE_VALGRIND=x)
endif(DEFINED USE_VALGRIND)
target_link_libraries(exchange_counter ext-boost pthread ${OPENSSL_LIBRARIES})