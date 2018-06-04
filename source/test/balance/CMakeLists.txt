project(test_coffee_balance)

file(GLOB SOURCES "*.cc")

add_executable(test_coffee_balance ${SOURCES})

target_link_libraries(test_coffee_balance coffee_balance coffee_logger coffee_xml coffee_basis coffee_config -lgtest -lxml2 ${CMAKE_THREAD_LIBS_INIT})

include_directories("../../include")



