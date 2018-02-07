project(coffee)

set(CMAKE_CXX_STANDARD 11)

cmake_minimum_required(VERSION 3.5)

add_subdirectory(source/config)
add_subdirectory(source/adt)
add_subdirectory(source/xml)
add_subdirectory(source/logger)
add_subdirectory(source/balance)
add_subdirectory(source/app)
add_subdirectory(source/dbms)
add_subdirectory(source/dbms.sqlite)
add_subdirectory(source/persistence)

add_subdirectory(test/config)
add_subdirectory(test/adt)
add_subdirectory(test/xml)
add_subdirectory(test/logger)
add_subdirectory(test/balance)
add_subdirectory(test/app)
add_subdirectory(test/dbms)
add_subdirectory(test/dbms.sqlite)
add_subdirectory(test/persistence)

