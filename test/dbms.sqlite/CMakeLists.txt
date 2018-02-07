project(test_coffee_dbms_sqlite)

cmake_minimum_required(VERSION 3.5)

file(GLOB SOURCES "*.cc")

add_executable(test_coffee_dbms_sqlite ${SOURCES})

target_link_libraries(test_coffee_dbms_sqlite coffee_dbms_sqlite coffee_dbms coffee_app coffee_balance coffee_logger coffee_xml coffee_adt coffee_config -lxml2 -lsqlite3  -lboost_system -lboost_filesystem)

include_directories("../../include")



