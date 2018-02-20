#include <iostream>
#include <time.h>

#include <boost/test/unit_test.hpp>

#include <coffee/dbms/datatype/String.hpp>
#include <coffee/dbms/datatype/Integer.hpp>

using namespace coffee;
using namespace coffee::dbms;

BOOST_AUTO_TEST_CASE(string_is_nulleable)
{
   datatype::String column("nulleable", 16, datatype::Constraint::CanBeNull);

   void* init = column.getBuffer();

   BOOST_REQUIRE_EQUAL(column.hasValue(), false);

   column.clear();

   BOOST_REQUIRE_EQUAL(column.hasValue(), false);
   BOOST_REQUIRE_THROW(column.getValue(), adt::RuntimeException);
   BOOST_REQUIRE_EQUAL(column.getSize(), 0);

   column.setValue("hello world");
   BOOST_REQUIRE_EQUAL(column.hasValue(), true);
   BOOST_REQUIRE_EQUAL(column.getSize(), coffee_strlen("hello world"));
   BOOST_REQUIRE_EQUAL(strncmp(column.getValue(), "hello world", coffee_strlen("hello world")), 0);

   column.clear();
   BOOST_REQUIRE_EQUAL(column.hasValue(), false);
   BOOST_REQUIRE_EQUAL(column.getSize(), 0);

   column.setValue("zzz");
   BOOST_REQUIRE_EQUAL(column.hasValue(), true);

   column.setValue(NULL);
   BOOST_REQUIRE_EQUAL(column.hasValue(), false);

   BOOST_REQUIRE_EQUAL(init, column.getBuffer());

   BOOST_REQUIRE_THROW(column.setValue("size out of range"), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(string_nulleable_asstring)
{
   datatype::String column("nulleable", 16, datatype::Constraint::CanBeNull);
   BOOST_REQUIRE_EQUAL(column.hasValue(), false);
   BOOST_REQUIRE_EQUAL(column.asString(), "datatype.String { datatype.Abstract { Name: nulleable | Null: true | Constraint: CanBeNull } | MaxSize: 16 | Value: <null> }");
}

BOOST_AUTO_TEST_CASE(string_is_not_nulleable)
{
   datatype::String column("not_nulleable", 4, datatype::Constraint::CanNotBeNull);

   void* init = column.getBuffer();

   BOOST_REQUIRE_EQUAL(column.hasValue(), true);

   column.setValue("bye");
   BOOST_REQUIRE_EQUAL(column.hasValue(), true);
   BOOST_REQUIRE_EQUAL(column.getSize(), coffee_strlen("bye"));
   BOOST_REQUIRE_EQUAL(strncmp(column.getValue(), "bye", coffee_strlen("bye")), 0);

   column.clear();
   BOOST_REQUIRE_EQUAL(column.hasValue(), true);
   BOOST_REQUIRE_EQUAL(column.getSize(), 0);

   BOOST_REQUIRE_EQUAL(init, column.getBuffer());

   BOOST_REQUIRE_THROW(column.setValue("size out of range"), adt::RuntimeException);
   BOOST_REQUIRE_THROW(column.setValue(NULL), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(string_downcast)
{
   datatype::String column("string_downcast", 4);

   datatype::Abstract& abs = column;

   const datatype::String& other = coffee_datatype_downcast(datatype::String, abs);
   column.setValue("123");

   BOOST_REQUIRE_EQUAL(other == column, true);

   datatype::Integer zzz("zzz");

   BOOST_REQUIRE_THROW(coffee_datatype_downcast(datatype::String, zzz), dbms::InvalidDataException);
}

BOOST_AUTO_TEST_CASE(string_clone)
{
   datatype::String cannotBeNull("cannotBeNull", 16, datatype::Constraint::CanNotBeNull);
   datatype::String canBeNull("canBeNull", 16, datatype::Constraint::CanBeNull);

   BOOST_REQUIRE_EQUAL(cannotBeNull.hasValue(), true);
   BOOST_REQUIRE_EQUAL(canBeNull.hasValue(), false);

   auto notnull(cannotBeNull.clone());
   auto null(canBeNull.clone());

   BOOST_REQUIRE_EQUAL(notnull->hasValue(), true);
   BOOST_REQUIRE_EQUAL(null->hasValue(), false);

   BOOST_REQUIRE_EQUAL(notnull->compare(cannotBeNull), 0);

   cannotBeNull.setValue("abcd");

   BOOST_REQUIRE_EQUAL(cannotBeNull.getValue(), "abcd");

   notnull = cannotBeNull.clone();
   BOOST_REQUIRE_EQUAL(notnull->hasValue(), true);
   BOOST_REQUIRE_EQUAL(notnull->compare(cannotBeNull), 0);

   canBeNull.setValue("xzy");
   null = canBeNull.clone();
   BOOST_REQUIRE_EQUAL(null->hasValue(), true);
   BOOST_REQUIRE_EQUAL(null->compare(canBeNull), 0);
   BOOST_REQUIRE_GT(null->compare(cannotBeNull), 0);

   BOOST_REQUIRE_LT(notnull->compare(canBeNull), 0);
}


