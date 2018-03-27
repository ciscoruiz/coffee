project(test_coffee_xml)

file(GLOB SOURCES "*.cc")

add_executable(test_coffee_xml ${SOURCES})

target_link_libraries(test_coffee_xml coffee_xml coffee_logger coffee_basis coffee_config -lxml2 -lboost_system -lboost_unit_test_framework -lboost_filesystem)

include_directories("../../include")



