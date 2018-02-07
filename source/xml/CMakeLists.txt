project(coffee_xml)

file(GLOB_RECURSE SOURCES "*.cc")

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/libs)

add_library(coffee_xml STATIC ${SOURCES})

include_directories("../../include")

if (COFFEE_XML_INCLUDE)
include_directories(${COFFEE_XML_INCLUDE})
else()
include_directories("/usr/include/libxml2")
endif()

if (COFFEE_BOOST_INCLUDE)
include_directories(${COFFEE_BOOST_INCLUDE})
endif()


