project(test_coffee_time)

file(GLOB SOURCES "*.cc")

add_executable(test_coffee_time ${SOURCES})

target_link_libraries(test_coffee_time coffee_time coffee_app coffee_balance coffee_logger coffee_xml coffee_basis coffee_config -lxml2 -lgtest -lboost_system -lboost_filesystem ${CMAKE_THREAD_LIBS_INIT})

include_directories("../../include")

