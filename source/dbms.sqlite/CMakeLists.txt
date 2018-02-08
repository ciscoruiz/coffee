project(coffee_dbms_sqlite)

file(GLOB SOURCES "*.cc")

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/libs)

add_library(coffee_dbms_sqlite STATIC ${SOURCES})

include_directories("../../include")

if (COFFEE_SQLITE3_PATH)
include_directories(${COFFEE_SQLITE3_PATH}/include)
endif()

