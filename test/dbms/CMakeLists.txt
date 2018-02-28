project(test_coffee_dbms)

file(GLOB SOURCES "*.cc")

add_executable(test_coffee_dbms ${SOURCES})

target_link_libraries(test_coffee_dbms coffee_dbms coffee_dbms_mock coffee_app coffee_balance coffee_logger coffee_xml coffee_adt coffee_config -lxml2 -lboost_system -lboost_filesystem)

include_directories("../../include")
include_directories("../dbms.mock")



