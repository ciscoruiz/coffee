project(coffee_config)

file(GLOB SOURCES "*.cc")

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/libs)

add_library(coffee_config STATIC ${SOURCES})

include_directories("../../include")



