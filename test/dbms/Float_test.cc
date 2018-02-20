#include <iostream>
#include <time.h>

#include <boost/test/unit_test.hpp>

#include <coffee/dbms/datatype/Float.hpp>
#include <coffee/dbms/datatype/Integer.hpp>

using namespace coffee;
using namespace coffee::dbms;


BOOST_AUTO_TEST_CASE(float_is_nulleable)
{
   datatype::Float column("nulleable", datatype::Constraint::CanBeNull);

   BOOST_REQUIRE_EQUAL(column.hasValue(), false);

   column.clear();

   BOOST_REQUIRE_EQUAL(column.hasValue(), false);

   BOOST_REQUIRE_THROW(column.getValue(), adt::RuntimeException);
   BOOST_REQUIRE_THROW(column.getFloatValue(), adt::RuntimeException);

   column.setValue(10.12);

   BOOST_REQUIRE_EQUAL(column.hasValue(), true);
   BOOST_REQUIRE_CLOSE(column.getValue(),10.12, 0.1);

   column.clear();
   BOOST_REQUIRE_EQUAL(column.hasValue(), false);;
}

BOOST_AUTO_TEST_CASE(float_is_not_nulleable)
{
   datatype::Float column("not_nulleable", datatype::Constraint::CanNotBeNull);

   BOOST_REQUIRE_EQUAL(column.hasValue(), true);

   column.setValue(0.0);
   BOOST_REQUIRE_EQUAL(column.hasValue(), true);
   BOOST_REQUIRE_EQUAL(column.getValue(), 0.0);

   column.clear();
   BOOST_REQUIRE_EQUAL(column.hasValue(), true);
}

BOOST_AUTO_TEST_CASE(float_downcast)
{
   datatype::Float column("float_downcast");

   datatype::Abstract& abs = column;

   BOOST_REQUIRE_EQUAL(abs.hasValue(), true);

   const datatype::Float& other = coffee_datatype_downcast(datatype::Float, abs);
   column.setValue(0.0006);

   BOOST_REQUIRE_EQUAL(other == column, true);

   datatype::Integer zzz("zzz");

   BOOST_REQUIRE_THROW(coffee_datatype_downcast(datatype::Float, zzz), dbms::InvalidDataException);
}

BOOST_AUTO_TEST_CASE(float_clone)
{
   datatype::Float cannotBeNull("cannotBeNull", datatype::Constraint::CanNotBeNull);
   datatype::Float canBeNull("canBeNull", datatype::Constraint::CanBeNull);

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

   BOOST_REQUIRE_EQUAL(null->compare(cannotBeNull), 1);

   BOOST_REQUIRE_EQUAL(notnull->compare(canBeNull), -1);
}
