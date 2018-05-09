project(coffee_dbms_mock)

file(GLOB_RECURSE SOURCES "*.cc")

set(LIBRARY_OUTPUT_PATH .)

add_library(coffee_dbms_mock STATIC ${SOURCES})

include_directories("../../include")



