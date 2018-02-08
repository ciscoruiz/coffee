project(test_coffee_logger)

file(GLOB SOURCES "*.cc")

add_executable(test_coffee_logger ${SOURCES})

target_link_libraries(test_coffee_logger coffee_logger coffee_xml coffee_adt coffee_config)

include_directories("../../include")



