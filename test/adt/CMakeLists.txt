project(test_coffee_adt)

cmake_minimum_required(VERSION 3.5)

file(GLOB SOURCES "*.cc")

add_executable(test_coffee_adt ${SOURCES})

target_link_libraries(test_coffee_adt coffee_adt coffee_config)

include_directories("../../include")



