project(coffee_http)

file(GLOB_RECURSE SOURCES "*.cc")

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/libs)

add_library(coffee_http STATIC ${SOURCES})

include_directories("../../include")



