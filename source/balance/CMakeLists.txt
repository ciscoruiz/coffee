project(coffee_balance)

file(GLOB SOURCES "*.cc")

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/libs)

add_library(coffee_balance STATIC ${SOURCES})

include_directories("../../include")

if (COFFEE_BOOST_INCLUDE)
include_directories(${COFFEE_BOOST_INCLUDE})
endif()


