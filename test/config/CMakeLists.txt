project(test_coffee_config)

cmake_minimum_required(VERSION 3.5)

file(GLOB SOURCES "*.cc")

add_executable(test_coffee_config ${SOURCES})

target_link_libraries(test_coffee_config coffee_config)

include_directories("../../include")



