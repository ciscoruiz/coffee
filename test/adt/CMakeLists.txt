project(test_coffee_adt)

file(GLOB SOURCES "*.cc")

add_executable(test_coffee_adt ${SOURCES})

target_link_libraries(test_coffee_adt coffee_adt coffee_config)

include_directories("../../include")



