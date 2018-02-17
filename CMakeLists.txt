project(coffee)

if (CMAKE_VERSION VERSION_LESS "3.1")
   set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
   set(CMAKE_CXX_STANDARD 11)
endif()

cmake_minimum_required(VERSION 2.8)

if (COFFEE_BOOST_PATH)
  include_directories(${COFFEE_BOOST_PATH}/include)
  link_directories(${COFFEE_BOOST_PATH}/lib)
endif()

if (COFFEE_XML_PATH)
  include_directories(${COFFEE_XML_PATH}/include/libxml2)
  link_directories(${COFFEE_XML_PATH}/lib)
endif()

set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)
SET(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage -fno-inline -fno-inline-small-functions -fno-default-inline")

SET(CMAKE_EXE_LINKER_FLAGS_COVERAGE "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -lgcov")

set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full -v")
set(MEMORYCHECK_SUPPRESSIONS_FILE ${PROJECT_SOURCE_DIR}/valgrind-suppression.txt)

add_subdirectory(source/config)
add_subdirectory(source/adt)
add_subdirectory(source/xml)
add_subdirectory(source/logger)
add_subdirectory(source/balance)
add_subdirectory(source/app)
add_subdirectory(source/dbms)
add_subdirectory(source/dbms.sqlite)
add_subdirectory(source/persistence)

enable_testing()

add_subdirectory(test/config)
add_subdirectory(test/adt)
add_subdirectory(test/xml)
add_subdirectory(test/logger)
add_subdirectory(test/balance)
add_subdirectory(test/app)
add_subdirectory(test/dbms.mock)
add_subdirectory(test/dbms)
add_subdirectory(test/dbms.sqlite)
add_subdirectory(test/persistence)

include(CTest)

add_test(test_coffee_config test/config/test_coffee_config)
add_test(test_coffee_adt test/adt/test_coffee_adt)
add_test(test_coffee_xml test/xml/test_coffee_xml)
add_test(test_coffee_logger test/logger/test_coffee_logger)
add_test(test_coffee_balance test/balance/test_coffee_balance)
add_test(test_coffee_app test/app/test_coffee_app)
add_test(test_coffee_dbms test/dbms/test_coffee_dbms)
add_test(test_coffee_dbms_sqlite test/dbms.sqlite/test_coffee_dbms_sqlite)
add_test(test_coffee_persistence test/persistence/test_coffee_persistence)

