#include <iostream>
#include <time.h>

#include <boost/test/unit_test.hpp>

#include <coffee/dbms/datatype/ShortBlock.hpp>
#include <coffee/dbms/datatype/Integer.hpp>

using namespace coffee;
using namespace coffee::dbms;


BOOST_AUTO_TEST_CASE (shortblock_is_nulleable)
{
   datatype::ShortBlock column ("nulleable", 128, datatype::Constraint::CanBeNull);

   void* init = column.getBuffer();

   BOOST_REQUIRE_EQUAL (column.hasValue (), false);

   column.clear ();

   BOOST_REQUIRE_EQUAL (column.hasValue (), false);

   adt::DataBlock other;
   BOOST_REQUIRE_THROW (column.getValue (), adt::RuntimeException);

   other.assign ("hello world", 7);

   BOOST_REQUIRE_EQUAL (other.size (), 7);

   column.setValue (other);
   BOOST_REQUIRE_EQUAL (column.hasValue (), true);
   BOOST_REQUIRE_EQUAL (column.getSize(), 7);
   BOOST_REQUIRE_EQUAL (strncmp(column.getValue().data(), "hello w", 7), 0);

   column.clear ();
   BOOST_REQUIRE_EQUAL (column.hasValue (), false);
   BOOST_REQUIRE_EQUAL(column.getSize(), 0);

   BOOST_REQUIRE_EQUAL(init, column.getBuffer());
}

BOOST_AUTO_TEST_CASE (shortblock_is_not_nulleable)
{
   datatype::ShortBlock column ("not_nulleable", 4, datatype::Constraint::CanNotBeNull);

   void* init = column.getBuffer();

   BOOST_REQUIRE_EQUAL (column.hasValue (), true);

   adt::DataBlock other ("hello world", 7);

   BOOST_REQUIRE_THROW(column.setValue (other), adt::RuntimeException);

   other.assign ("hello", 4);
   column.setValue (other);
   BOOST_REQUIRE_EQUAL (column.hasValue (), true);
   BOOST_REQUIRE_EQUAL (column.getSize(), 4);
   BOOST_REQUIRE_EQUAL (strncmp(column.getValue().data(), "hello w", 4), 0);

   column.clear();
   BOOST_REQUIRE_EQUAL (column.hasValue(), true);
   BOOST_REQUIRE_EQUAL(column.getSize(), 0);

   BOOST_REQUIRE_EQUAL(init, column.getBuffer());

   other.assign ("size out of range");
   BOOST_REQUIRE_THROW (column.setValue (other), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE (shortblock_downcast)
{
   datatype::ShortBlock column ("not_nulleable", 4);

   datatype::Abstract& abs = column;

   auto other = coffee_datatype_downcast(datatype::ShortBlock, abs);

   datatype::Integer zzz ("zzz");

   BOOST_REQUIRE_THROW(coffee_datatype_downcast(datatype::ShortBlock, zzz), dbms::InvalidDataException);
}

BOOST_AUTO_TEST_CASE (shortblock_clone)
{
   datatype::ShortBlock column ("not_nulleable", 1025);

   const char* buffer = new char[1024];
   adt::DataBlock memory(buffer, 1024);

   column.setValue(memory);

   auto clone = coffee_datatype_downcast(datatype::ShortBlock, column.clone());

   BOOST_REQUIRE(clone->getValue() == column.getValue());
}
