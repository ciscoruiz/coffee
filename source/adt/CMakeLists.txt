project(coffee_adt)

cmake_minimum_required(VERSION 3.5)

file(GLOB_RECURSE SOURCES "*.cc")

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/libs)

add_library(coffee_adt STATIC ${SOURCES})

include_directories("../../include")



