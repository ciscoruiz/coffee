project(test_coffee_config)

file(GLOB SOURCES "*.cc")

add_executable(test_coffee_config ${SOURCES})

target_link_libraries(test_coffee_config coffee_config)

include_directories("../../include")



