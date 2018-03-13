project(test_coffee_time)

file(GLOB SOURCES "*.cc")

add_executable(test_coffee_time ${SOURCES})

target_link_libraries(test_coffee_time coffee_time coffee_app coffee_balance coffee_logger coffee_xml coffee_adt coffee_config -lxml2 -lpthread -lboost_system -lboost_filesystem)

include_directories("../../include")



