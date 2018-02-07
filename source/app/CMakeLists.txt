project(coffee_app)

cmake_minimum_required(VERSION 3.5)

file(GLOB SOURCES "*.cc")

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/libs)

add_library(coffee_app STATIC ${SOURCES})

include_directories("../../include")



