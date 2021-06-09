include(ExternalProject)

if (NOT APPLE)
    add_subdirectory(boost)
endif()

set(SIMDJSON_JUST_LIBRARY ON)
set(SIMDJSON_BUILD_STATIC ON)
add_subdirectory(simdjson)