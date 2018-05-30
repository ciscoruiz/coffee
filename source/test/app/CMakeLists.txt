project(test_coffee_app)

file(GLOB SOURCES "*.cc")

add_executable(test_coffee_app ${SOURCES})

target_link_libraries(test_coffee_app coffee_app coffee_balance coffee_logger coffee_xml coffee_basis coffee_config -lgtest -lxml2 -lboost_system -lboost_filesystem ${CMAKE_THREAD_LIBS_INIT})

include_directories("../../include")



