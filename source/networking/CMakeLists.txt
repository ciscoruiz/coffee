project(coffee_networking)

file(GLOB_RECURSE SOURCES "*.cc")

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/libs)

add_library(coffee_networking STATIC ${SOURCES})

include_directories("../../include")



