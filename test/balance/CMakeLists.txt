project(test_coffee_balance)

cmake_minimum_required(VERSION 3.5)

file(GLOB SOURCES "*.cc")

add_executable(test_coffee_balance ${SOURCES})

target_link_libraries(test_coffee_balance coffee_balance coffee_logger coffee_xml coffee_adt coffee_config -lxml2 -lpthread)

include_directories("../../include")



