project(test_coffee_basis)

file(GLOB SOURCES "*.cc")

add_executable(test_coffee_basis ${SOURCES})

target_link_libraries(test_coffee_basis coffee_basis coffee_config)

include_directories("../../include")



