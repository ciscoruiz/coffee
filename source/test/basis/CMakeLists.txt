project(test_coffee_basis)

file(GLOB SOURCES "*.cc")

add_executable(test_coffee_basis ${SOURCES})

target_link_libraries(test_coffee_basis -lgtest coffee_basis coffee_config ${CMAKE_THREAD_LIBS_INIT})

include_directories("../../include")



