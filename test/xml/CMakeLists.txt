project(test_coffee_xml)

cmake_minimum_required(VERSION 3.5)

file(GLOB SOURCES "*.cc")

add_executable(test_coffee_xml ${SOURCES})

target_link_libraries(test_coffee_xml coffee_xml coffee_logger coffee_adt coffee_config -lxml2 -lboost_system -lboost_unit_test_framework -lboost_filesystem)

include_directories("../../include")



