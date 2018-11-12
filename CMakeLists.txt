project(coffee)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")

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

# first we can indicate the documentation build as an option and set it to ON by default
option(BUILD_DOC "Build documentation" OFF)

# Thanks to https://vicrucann.github.io/tutorials/quick-cmake-doxygen/
# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${PROJECT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${PROJECT_SOURCE_DIR}/docs/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target(doc 
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

find_path(LDAP NAMES "ldap.h")
find_path(ZMQ NAMES "zmq.hpp")
find_path(GTEST NAMES "gtest")

add_subdirectory(source/src/config)
add_subdirectory(source/src/basis)
add_subdirectory(source/src/xml)
add_subdirectory(source/src/logger)
add_subdirectory(source/src/balance)
add_subdirectory(source/src/app)
add_subdirectory(source/src/time)
add_subdirectory(source/src/dbms)
add_subdirectory(source/src/dbms.sqlite)

if (LDAP STREQUAL "LDAP-NOTFOUND")
   message ("ldap.h was not found, it will not compile LDAP module")
else (LDAP STREQUAL "LDAP-NOTFOUND")
   add_subdirectory(source/src/dbms.ldap)
endif (LDAP STREQUAL "LDAP-NOTFOUND")

add_subdirectory(source/src/persistence)

if (ZMQ STREQUAL "ZMQ-NOTFOUND")
   message ("zmq.hpp was not found, it will not compile Networking module, nor HTTP module")
else (ZMQ STREQUAL "ZMQ-NOTFOUND")
   add_subdirectory(source/src/networking)
   add_subdirectory(source/src/http)
endif (ZMQ STREQUAL "ZMQ-NOTFOUND")

if (COFFEE_NO_UNITTEST)
   message ("Unit Test has been deactivated. it will not run any unit test")
else (COFFEE_NO_UNITTEST)
   find_package (Threads)
   enable_testing()
   
   link_directories(${PROJECT_SOURCE_DIR}/libs/gtest)
   
   add_subdirectory(source/test/config)
   add_subdirectory(source/test/basis)
   add_subdirectory(source/test/xml)
   add_subdirectory(source/test/logger)
   add_subdirectory(source/test/balance)
   add_subdirectory(source/test/app)
   add_subdirectory(source/test/time)
   add_subdirectory(source/test/dbms.mock)
   add_subdirectory(source/test/dbms)
   add_subdirectory(source/test/dbms.sqlite)
   
   if (NOT LDAP STREQUAL "LDAP-NOTFOUND")
      add_subdirectory(source/test/dbms.ldap)
   endif (NOT LDAP STREQUAL "LDAP-NOTFOUND")
   
   add_subdirectory(source/test/persistence)
   
   if (NOT ZMQ STREQUAL "ZMQ-NOTFOUND")
      add_subdirectory(source/test/networking)
      add_subdirectory(source/test/http)
   endif (NOT ZMQ STREQUAL "ZMQ-NOTFOUND")
   
   include(CTest)
   
   add_test(test_coffee_config source/test/config/test_coffee_config)
   add_test(test_coffee_basis source/test/basis/test_coffee_basis)
   add_test(test_coffee_xml source/test/xml/test_coffee_xml)
   add_test(test_coffee_logger source/test/logger/test_coffee_logger)
   add_test(test_coffee_balance source/test/balance/test_coffee_balance)
   add_test(test_coffee_app source/test/app/test_coffee_app)
   add_test(test_coffee_dbms source/test/dbms/test_coffee_dbms)
   add_test(test_coffee_time source/test/time/test_coffee_time)
   add_test(test_coffee_dbms_sqlite source/test/dbms.sqlite/test_coffee_dbms_sqlite)
   
   if (NOT LDAP STREQUAL "LDAP-NOTFOUND")
      add_test(test_coffee_dbms_ldap source/test/dbms.ldap/test_coffee_dbms_ldap)
   endif (NOT LDAP STREQUAL "LDAP-NOTFOUND")
   
   add_test(test_coffee_persistence source/test/persistence/test_coffee_persistence)
   
   if (NOT ZMQ STREQUAL "ZMQ-NOTFOUND")
      add_test(test_coffee_networking source/test/networking/test_coffee_networking)
      add_test(test_coffee_http source/test/http/test_coffee_http)
   endif (NOT ZMQ STREQUAL "ZMQ-NOTFOUND")
endif (COFFEE_NO_UNITTEST)
