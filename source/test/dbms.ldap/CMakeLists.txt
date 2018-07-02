project(test_coffee_dbms_ldap)

file(GLOB SOURCES "*.cc")

add_executable(test_coffee_dbms_ldap ${SOURCES})

target_link_libraries(test_coffee_dbms_ldap coffee_dbms_ldap coffee_dbms coffee_time coffee_app coffee_balance coffee_logger coffee_xml coffee_basis coffee_config -lxml2 -lldap -llber -lgtest -lboost_system ${CMAKE_THREAD_LIBS_INIT})

include_directories("../../include")



