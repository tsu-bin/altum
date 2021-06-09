cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(altum)

set(CMAKE_CXX_FLAGS_DEBUG "-g -std=c++17")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g")
set(CMAKE_CXX_FLAGS_SANITIZER "-fsanitize=address -fsanitize=leak -g") # cmake -DCMAKE_BUILD_TYPE=Debug/Release/SANITIZER ../altum

set(CMAKE_CXX_FLAGS "-std=c++17 -fconcepts")

if (NOT APPLE)
    find_package(OpenSSL REQUIRED)
else()
    add_subdirectory(headers_for_mac_ide)
endif()

if(DEFINED USE_VALGRIND) # cmake -DCMAKE_BUILD_TYPE=Debug -DUSE_VALGRIND=x ../altum
    set(boost_b2_valgrind_param "valgrind=on")
endif(DEFINED USE_VALGRIND)


add_subdirectory(thirdparty)


add_subdirectory(src)

add_subdirectory(demo)