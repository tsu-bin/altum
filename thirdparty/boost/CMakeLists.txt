set(prefix ${CMAKE_CURRENT_BINARY_DIR})

# Comes in three flavors:
# - ext-boost, the full version, with all static libraries
# - ext-boost-headers, the header-only version
# - ext-boost-<component>, one for each .a library
#
# But beware that ext-boost-headers also depends on the full
# version being built.  In other words, linking to
# ext-boost-headers does not save you any compilation time.

set(_boost_components
        #asio only headers
        #beast is header only
        #chrono
        #config

        coroutine # coroutine 依赖 thread
        thread # thread 须放在 coroutine 后面
        context
        #date_time
        #filesystem
        #lockfree
        #system
        )

ExternalProject_Add(ext-ep-boost
        PREFIX ${prefix}
        #URL https://boostorg.jfrog.io/artifactory/main/release/1.76.0/source/boost_1_76_0.tar.gz
        URL ${CMAKE_CURRENT_SOURCE_DIR}/boost_1_76_0_doc_removed.tar.gz
        #URL_MD5 61338d813639864b499852c5575be0ed
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND ./bootstrap.sh --prefix=${prefix} --without-icu --without-libraries=python
        BUILD_COMMAND ":"
        INSTALL_COMMAND ./b2 ${_pic_arg} -s NO_BZIP2=1 link=static ${boost_b2_valgrind_param} --layout=system install)

add_library(ext-boost-headers INTERFACE)
add_dependencies(ext-boost-headers ext-ep-boost)
target_include_directories(ext-boost-headers SYSTEM INTERFACE ${prefix}/include)

add_library(ext-boost INTERFACE)
add_dependencies(ext-boost ext-ep-boost)
target_include_directories(ext-boost SYSTEM INTERFACE ${prefix}/include)
foreach (component IN LISTS _boost_components)
    set(_lib ${prefix}/lib/libboost_${component}.a)
    target_link_libraries(ext-boost INTERFACE ${_lib})

    add_library(ext-boost-${component} INTERFACE)
    add_dependencies(ext-boost-${component} ext-ep-boost)
    target_include_directories(ext-boost-${component} SYSTEM INTERFACE ${prefix}/include)
    target_link_libraries(ext-boost-${component} INTERFACE ${_lib})
endforeach()

