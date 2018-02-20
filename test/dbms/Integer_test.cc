#include <iostream>
#include <time.h>

#include <boost/test/unit_test.hpp>

#include <coffee/dbms/datatype/Integer.hpp>
#include <coffee/dbms/datatype/Float.hpp>

using namespace coffee;
using namespace coffee::dbms;


BOOST_AUTO_TEST_CASE(integer_is_nulleable)
{
   datatype::Integer column("nulleable", datatype::Constraint::CanBeNull);

   BOOST_REQUIRE_EQUAL(column.hasValue(), false);

   column.clear();

   BOOST_REQUIRE_EQUAL(column.hasValue(), false);

   BOOST_REQUIRE_THROW(column.getValue(), adt::RuntimeException);

   column.setValue(10);
   BOOST_REQUIRE_EQUAL(column.hasValue(), true);
   BOOST_REQUIRE_EQUAL(column.getValue(),10);

   column.clear();
   BOOST_REQUIRE_EQUAL(column.hasValue(), false);
}

BOOST_AUTO_TEST_CASE(integer_is_not_nulleable)
{
   datatype::Integer column("not_nulleable", datatype::Constraint::CanNotBeNull);

   BOOST_REQUIRE_EQUAL(column.hasValue(), true);

   column.setValue(11);
   BOOST_REQUIRE_EQUAL(column.hasValue(), true);
   BOOST_REQUIRE_EQUAL(column.getValue(), 11);

   column.clear();
   BOOST_REQUIRE_EQUAL(column.hasValue(), true);
}

BOOST_AUTO_TEST_CASE(integer_downcast)
{
   datatype::Integer column("integer_downcast");

   datatype::Abstract& abs = column;

   const datatype::Integer& other = coffee_datatype_downcast(datatype::Integer, abs);
   column.setValue(1234);

   BOOST_REQUIRE_EQUAL(other == column, true);

   datatype::Float zzz("zzz");

   BOOST_REQUIRE_THROW(coffee_datatype_downcast(datatype::Integer, zzz), dbms::InvalidDataException);
}

BOOST_AUTO_TEST_CASE(integer_clone)
{
   datatype::Integer cannotBeNull("cannotBeNull", datatype::Constraint::CanNotBeNull);
   datatype::Integer canBeNull("canBeNull", datatype::Constraint::CanBeNull);

   BOOST_REQUIRE_EQUAL(cannotBeNull.hasValue(), true);
   BOOST_REQUIRE_EQUAL(canBeNull.hasValue(), false);

   auto notnull(cannotBeNull.clone());
   auto null(canBeNull.clone());

   BOOST_REQUIRE_EQUAL(notnull->hasValue(), true);
   BOOST_REQUIRE_EQUAL(null->hasValue(), false);

   BOOST_REQUIRE_EQUAL(notnull->compare(cannotBeNull), 0);

   cannotBeNull.setValue(5.0);

   BOOST_REQUIRE_EQUAL(cannotBeNull.getValue(), 5.0);

   notnull = cannotBeNull.clone();
   BOOST_REQUIRE_EQUAL(notnull->hasValue(), true);
   BOOST_REQUIRE_EQUAL(notnull->compare(cannotBeNull), 0);

   canBeNull.setValue(25);
   null = canBeNull.clone();
   BOOST_REQUIRE_EQUAL(null->hasValue(), true);
   BOOST_REQUIRE_EQUAL(null->compare(canBeNull), 0);

   BOOST_REQUIRE_EQUAL(null->compare(cannotBeNull), 20);

   BOOST_REQUIRE_EQUAL(notnull->compare(canBeNull), -20);
}
