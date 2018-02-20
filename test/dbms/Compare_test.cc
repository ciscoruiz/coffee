
#include <boost/test/unit_test.hpp>

#include <coffee/dbms/datatype/Date.hpp>
#include <coffee/dbms/datatype/Float.hpp>
#include <coffee/dbms/datatype/Integer.hpp>
#include <coffee/dbms/datatype/LongBlock.hpp>
#include <coffee/dbms/datatype/ShortBlock.hpp>
#include <coffee/dbms/datatype/String.hpp>
#include <coffee/dbms/datatype/TimeStamp.hpp>

using namespace coffee;

BOOST_AUTO_TEST_CASE(dbms_compare_date)
{
   dbms::datatype::Date value("value", dbms::datatype::Constraint::CanBeNull);
   dbms::datatype::Date other("other", dbms::datatype::Constraint::CanBeNull);

   BOOST_REQUIRE_EQUAL(value.hasValue(), false);
   BOOST_REQUIRE_EQUAL(other.hasValue(), false);

   BOOST_REQUIRE_EQUAL(value.compare(other), 0);

   other.setValue(adt::Second::getTime());

   BOOST_REQUIRE_LT(value.compare(other), 0);
   BOOST_REQUIRE_GT(other.compare(value), 0);

   value.setValue(other.getValue());
   BOOST_REQUIRE_EQUAL(value.compare(other), 0);

   adt::Second second = adt::Second(other.getValue() + 10);
   value.setValue(second);
   BOOST_REQUIRE_GT(value.compare(other), 0);
   BOOST_REQUIRE_LT(other.compare(value), 0);

   other.clear();
   BOOST_REQUIRE_EQUAL(other.hasValue(), false);
   BOOST_REQUIRE_GT(value.compare(other), 0);
   BOOST_REQUIRE_LT(other.compare(value), 0);

   dbms::datatype::Float theFloat("theFloat", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theFloat), adt::RuntimeException);

   dbms::datatype::Integer theInteger("theInteger", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theInteger), adt::RuntimeException);

   dbms::datatype::LongBlock theLongBlock("theLongBlock", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theLongBlock), adt::RuntimeException);

   dbms::datatype::ShortBlock theShortBlock("theShortBlock", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theShortBlock), adt::RuntimeException);

   dbms::datatype::String theString("theString", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theString), adt::RuntimeException);

   dbms::datatype::TimeStamp theTimeStamp("theTimeStamp", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theTimeStamp), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(dbms_compare_float)
{
   dbms::datatype::Float value("value", dbms::datatype::Constraint::CanBeNull);
   dbms::datatype::Float other("other", dbms::datatype::Constraint::CanBeNull);

   BOOST_REQUIRE_EQUAL(value.hasValue(), false);
   BOOST_REQUIRE_EQUAL(other.hasValue(), false);

   BOOST_REQUIRE_EQUAL(value.compare(other), 0);

   other.setValue(0.111);

   BOOST_REQUIRE_LT(value.compare(other), 0);
   BOOST_REQUIRE_GT(other.compare(value), 0);

   value.setValue(other.getValue());
   BOOST_REQUIRE_EQUAL(value.compare(other), 0);

   value.setValue(0.122);
   BOOST_REQUIRE_GT(value.compare(other), 0);

   other.clear();
   BOOST_REQUIRE_EQUAL(other.hasValue(), false);
   BOOST_REQUIRE_GT(value.compare(other), 0);
   BOOST_REQUIRE_LT(other.compare(value), 0);

   dbms::datatype::Date theDate("theDate", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theDate), adt::RuntimeException);

   dbms::datatype::Integer theInteger("theInteger", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theInteger), adt::RuntimeException);

   dbms::datatype::LongBlock theLongBlock("theLongBlock", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theLongBlock), adt::RuntimeException);

   dbms::datatype::ShortBlock theShortBlock("theShortBlock", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theShortBlock), adt::RuntimeException);

   dbms::datatype::String theString("theString", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theString), adt::RuntimeException);

   dbms::datatype::TimeStamp theTimeStamp("theTimeStamp", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theTimeStamp), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(dbms_compare_float_abstract)
{
   dbms::datatype::Float value("value", dbms::datatype::Constraint::CanBeNull);
   dbms::datatype::Float value2("other", dbms::datatype::Constraint::CanBeNull);

   value.setValue(0.123);
   value2.setValue(0.0);

   dbms::datatype::Abstract& abstract(value2);

   BOOST_REQUIRE_NE(value.compare(abstract), 0);
}

BOOST_AUTO_TEST_CASE(dbms_compare_integer)
{
   dbms::datatype::Integer value("value", dbms::datatype::Constraint::CanBeNull);
   dbms::datatype::Integer other("other", dbms::datatype::Constraint::CanBeNull);

   BOOST_REQUIRE_EQUAL(value.hasValue(), false);
   BOOST_REQUIRE_EQUAL(other.hasValue(), false);

   BOOST_REQUIRE_EQUAL(value.compare(other), 0);

   other.setValue(111);

   BOOST_REQUIRE_LT(value.compare(other), 0);
   BOOST_REQUIRE_GT(other.compare(value), 0);

   value.setValue(other.getValue());
   BOOST_REQUIRE_EQUAL(value.compare(other), 0);

   value.setValue(122);
   BOOST_REQUIRE_GT(value.compare(other), 0);

   other.clear();
   BOOST_REQUIRE_EQUAL(other.hasValue(), false);
   BOOST_REQUIRE_GT(value.compare(other), 0);
   BOOST_REQUIRE_LT(other.compare(value), 0);

   dbms::datatype::Date theDate("theDate", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theDate), adt::RuntimeException);

   dbms::datatype::Float theFloat("theFloat", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theFloat), adt::RuntimeException);

   dbms::datatype::LongBlock theLongBlock("theLongBlock", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theLongBlock), adt::RuntimeException);

   dbms::datatype::ShortBlock theShortBlock("theShortBlock", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theShortBlock), adt::RuntimeException);

   dbms::datatype::String theString("theString", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theString), adt::RuntimeException);

   dbms::datatype::TimeStamp theTimeStamp("theTimeStamp", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theTimeStamp), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(dbms_compare_string)
{
   dbms::datatype::String value("value", 16, dbms::datatype::Constraint::CanBeNull);
   dbms::datatype::String other("other", 16, dbms::datatype::Constraint::CanBeNull);

   BOOST_REQUIRE_EQUAL(value.hasValue(), false);
   BOOST_REQUIRE_EQUAL(other.hasValue(), false);

   BOOST_REQUIRE_EQUAL(value.compare(other), 0);

   other.setValue("hello");

   BOOST_REQUIRE_LT(value.compare(other), 0);
   BOOST_REQUIRE_GT(other.compare(value), 0);

   value.setValue(other.getValue());
   BOOST_REQUIRE_EQUAL(value.compare(other), 0);

   value.setValue("xabc");
   BOOST_REQUIRE_GT(value.compare(other), 0);
   BOOST_REQUIRE_LT(other.compare(value), 0);

   other.clear();
   BOOST_REQUIRE_EQUAL(other.hasValue(), false);
   BOOST_REQUIRE_GT(value.compare(other), 0);
   BOOST_REQUIRE_LT(other.compare(value), 0);

   dbms::datatype::Date theDate("theDate", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theDate), adt::RuntimeException);

   dbms::datatype::Float theFloat("theFloat", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theFloat), adt::RuntimeException);

   dbms::datatype::Integer theInteger("theInteger", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theInteger), adt::RuntimeException);

   dbms::datatype::LongBlock theLongBlock("theLongBlock", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theLongBlock), adt::RuntimeException);

   dbms::datatype::ShortBlock theShortBlock("theShortBlock", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theShortBlock), adt::RuntimeException);

   dbms::datatype::TimeStamp theTimeStamp("theTimeStamp", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theTimeStamp), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(dbms_compare_longblock)
{
   dbms::datatype::LongBlock value("value", dbms::datatype::Constraint::CanBeNull);
   dbms::datatype::LongBlock other("other", dbms::datatype::Constraint::CanBeNull);

   BOOST_REQUIRE_EQUAL(value.hasValue(), false);
   BOOST_REQUIRE_EQUAL(other.hasValue(), false);

   BOOST_REQUIRE_EQUAL(value.compare(other), 0);

   adt::DataBlock datablock;

   datablock.append("hello");
   other.setValue(datablock);

   BOOST_REQUIRE_LT(value.compare(other), 0);
   BOOST_REQUIRE_GT(other.compare(value), 0);

   value.setValue(other.getValue());
   BOOST_REQUIRE_THROW(value.compare(other), adt::RuntimeException);

   datablock = "xabc";
   value.setValue(datablock);
   BOOST_REQUIRE_THROW(value.compare(other), adt::RuntimeException);
   BOOST_REQUIRE_THROW(other.compare(value), adt::RuntimeException);

   other.clear();
   BOOST_REQUIRE_EQUAL(other.hasValue(), false);
   BOOST_REQUIRE_GT(value.compare(other), 0);
   BOOST_REQUIRE_LT(other.compare(value), 0);

   dbms::datatype::Date theDate("theDate", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theDate), adt::RuntimeException);

   dbms::datatype::Float theFloat("theFloat", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theFloat), adt::RuntimeException);

   dbms::datatype::Integer theInteger("theInteger", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theInteger), adt::RuntimeException);

   dbms::datatype::String theString("theString", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theString), adt::RuntimeException);

   dbms::datatype::ShortBlock theShortBlock("theShortBlock", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theShortBlock), adt::RuntimeException);

   dbms::datatype::TimeStamp theTimeStamp("theTimeStamp", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theTimeStamp), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(dbms_compare_shortblock)
{
   dbms::datatype::ShortBlock value("value", 16, dbms::datatype::Constraint::CanBeNull);
   dbms::datatype::ShortBlock other("other", 16, dbms::datatype::Constraint::CanBeNull);

   BOOST_REQUIRE_EQUAL(value.hasValue(), false);
   BOOST_REQUIRE_EQUAL(other.hasValue(), false);

   BOOST_REQUIRE_EQUAL(value.compare(other), 0);

   adt::DataBlock datablock;

   datablock.append("hello");
   other.setValue(datablock);

   BOOST_REQUIRE_LT(value.compare(other), 0);
   BOOST_REQUIRE_GT(other.compare(value), 0);

   value.setValue(other.getValue());
   BOOST_REQUIRE_THROW(value.compare(other), adt::RuntimeException);

   datablock = "xabc";
   value.setValue(datablock);
   BOOST_REQUIRE_THROW(value.compare(other), adt::RuntimeException);
   BOOST_REQUIRE_THROW(other.compare(value), adt::RuntimeException);

   other.clear();
   BOOST_REQUIRE_EQUAL(other.hasValue(), false);
   BOOST_REQUIRE_GT(value.compare(other), 0);
   BOOST_REQUIRE_LT(other.compare(value), 0);

   dbms::datatype::Date theDate("theDate", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theDate), adt::RuntimeException);

   dbms::datatype::Float theFloat("theFloat", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theFloat), adt::RuntimeException);

   dbms::datatype::Integer theInteger("theInteger", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theInteger), adt::RuntimeException);

   dbms::datatype::String theString("theString", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theString), adt::RuntimeException);

   dbms::datatype::LongBlock theShortBlock("theShortBlock", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theShortBlock), adt::RuntimeException);

   dbms::datatype::TimeStamp theTimeStamp("theTimeStamp", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theTimeStamp), adt::RuntimeException);
}



