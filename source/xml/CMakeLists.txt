project(coffee_xml)

cmake_minimum_required(VERSION 3.5)

file(GLOB_RECURSE SOURCES "*.cc")

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/libs)

add_library(coffee_xml STATIC ${SOURCES})

include_directories("../../include")
include_directories("/usr/include/libxml2")



