project(test_coffee_http)

file(GLOB SOURCES "*.cc")

add_executable(test_coffee_http ${SOURCES})

target_link_libraries(test_coffee_http coffee_http coffee_xml coffee_logger coffee_basis coffee_config)

include_directories("../../include")



