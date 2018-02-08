project(test_coffee_app)

file(GLOB SOURCES "*.cc")

add_executable(test_coffee_app ${SOURCES})

target_link_libraries(test_coffee_app coffee_app coffee_balance coffee_logger coffee_xml coffee_adt coffee_config -lxml2)

include_directories("../../include")



