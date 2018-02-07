project(test_coffee_logger)

cmake_minimum_required(VERSION 3.5)

file(GLOB SOURCES "*.cc")

add_executable(test_coffee_logger ${SOURCES})

target_link_libraries(test_coffee_logger coffee_logger coffee_xml coffee_adt coffee_config)

include_directories("../../include")



