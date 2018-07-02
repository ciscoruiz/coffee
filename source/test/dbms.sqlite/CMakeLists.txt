project(test_coffee_dbms_sqlite)

file(GLOB SOURCES "*.cc")

if (COFFEE_SQLITE3_PATH)
include_directories(${COFFEE_SQLITE3_PATH}/include)
link_directories(${COFFEE_SQLITE3_PATH}/lib)
endif()

add_executable(test_coffee_dbms_sqlite ${SOURCES})

target_link_libraries(test_coffee_dbms_sqlite coffee_dbms_sqlite coffee_dbms coffee_time coffee_app coffee_balance coffee_logger coffee_xml coffee_basis coffee_config -lxml2 -lsqlite3 -lgtest -lboost_system -lboost_filesystem ${CMAKE_THREAD_LIBS_INIT})

include_directories("../../include")



