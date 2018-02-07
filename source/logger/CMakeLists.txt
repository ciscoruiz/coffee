project(coffee_logger)

file(GLOB_RECURSE SOURCES "*.cc")

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/libs)

add_library(coffee_logger STATIC ${SOURCES})

include_directories("../../include")



