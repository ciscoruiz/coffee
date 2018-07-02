project(test_coffee_xml)

file(GLOB SOURCES "*.cc")

add_executable(test_coffee_xml ${SOURCES})

target_link_libraries(test_coffee_xml coffee_xml coffee_logger coffee_basis coffee_config -lxml2 -lboost_system -lgtest -lboost_filesystem ${CMAKE_THREAD_LIBS_INIT})

include_directories("../../include")



