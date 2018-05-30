project(test_coffee_config)

file(GLOB SOURCES "*.cc")

add_executable(test_coffee_config ${SOURCES})

target_link_libraries(test_coffee_config -lgtest coffee_config ${CMAKE_THREAD_LIBS_INIT} )

include_directories("../../include")

