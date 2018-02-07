project(test_coffee_dbms_sqlite)

file(GLOB SOURCES "*.cc")

if (COFFEE_SQLITE3_PATH)
include_directories(${COFFEE_SQLITE3_PATH}/include)
link_directories(${COFFEE_SQLITE3_PATH}/lib)
endif()

add_executable(test_coffee_dbms_sqlite ${SOURCES})

target_link_libraries(test_coffee_dbms_sqlite coffee_dbms_sqlite coffee_dbms coffee_app coffee_balance coffee_logger coffee_xml coffee_adt coffee_config -lxml2 -lsqlite3  -lboost_system -lboost_filesystem)

include_directories("../../include")



