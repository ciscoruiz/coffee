project(test_coffee_persistence)

file(GLOB SOURCES "*.cc")

add_executable(test_coffee_persistence ${SOURCES})

target_link_libraries(test_coffee_persistence coffee_persistence coffee_dbms coffee_dbms_mock coffee_app coffee_balance coffee_logger coffee_xml coffee_adt coffee_config coffee_dbms_mock -lxml2)

include_directories("../../include")
include_directories("../dbms.mock")

