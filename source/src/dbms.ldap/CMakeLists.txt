project(coffee_dbms_ldap)

file(GLOB SOURCES "*.cc")

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/libs)

add_library(coffee_dbms_ldap STATIC ${SOURCES})

include_directories("../../include")

