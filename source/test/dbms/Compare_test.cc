// MIT License
// 
// Copyright (c) 2018 Francisco Ruiz (francisco.ruiz.rayo@gmail.com)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//


#include <boost/test/unit_test.hpp>

#include <coffee/time/TimeService.hpp>

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

   auto now = time::TimeService::toSeconds(time::TimeService::now());

   other.setValue(now);

   BOOST_REQUIRE_LT(value.compare(other), 0);
   BOOST_REQUIRE_GT(other.compare(value), 0);

   value.setValue(other.getValue());
   BOOST_REQUIRE_EQUAL(value.compare(other), 0);

   value.setValue(now + std::chrono::seconds(10));
   BOOST_REQUIRE_GT(value.compare(other), 0);
   BOOST_REQUIRE_LT(other.compare(value), 0);

   other.clear();
   BOOST_REQUIRE(!other.hasValue());
   BOOST_REQUIRE_GT(value.compare(other), 0);
   BOOST_REQUIRE_LT(other.compare(value), 0);

   dbms::datatype::Float theFloat("theFloat", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theFloat), basis::RuntimeException);

   dbms::datatype::Integer theInteger("theInteger", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theInteger), basis::RuntimeException);

   dbms::datatype::LongBlock theLongBlock("theLongBlock", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theLongBlock), basis::RuntimeException);

   dbms::datatype::ShortBlock theShortBlock("theShortBlock", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theShortBlock), basis::RuntimeException);

   dbms::datatype::String theString("theString", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theString), basis::RuntimeException);

   dbms::datatype::TimeStamp theTimeStamp("theTimeStamp", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theTimeStamp), basis::RuntimeException);
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
   BOOST_REQUIRE_THROW(value.compare(theDate), basis::RuntimeException);

   dbms::datatype::Integer theInteger("theInteger", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theInteger), basis::RuntimeException);

   dbms::datatype::LongBlock theLongBlock("theLongBlock", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theLongBlock), basis::RuntimeException);

   dbms::datatype::ShortBlock theShortBlock("theShortBlock", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theShortBlock), basis::RuntimeException);

   dbms::datatype::String theString("theString", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theString), basis::RuntimeException);

   dbms::datatype::TimeStamp theTimeStamp("theTimeStamp", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theTimeStamp), basis::RuntimeException);
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
   BOOST_REQUIRE_THROW(value.compare(theDate), basis::RuntimeException);

   dbms::datatype::Float theFloat("theFloat", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theFloat), basis::RuntimeException);

   dbms::datatype::LongBlock theLongBlock("theLongBlock", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theLongBlock), basis::RuntimeException);

   dbms::datatype::ShortBlock theShortBlock("theShortBlock", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theShortBlock), basis::RuntimeException);

   dbms::datatype::String theString("theString", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theString), basis::RuntimeException);

   dbms::datatype::TimeStamp theTimeStamp("theTimeStamp", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theTimeStamp), basis::RuntimeException);
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
   BOOST_REQUIRE_THROW(value.compare(theDate), basis::RuntimeException);

   dbms::datatype::Float theFloat("theFloat", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theFloat), basis::RuntimeException);

   dbms::datatype::Integer theInteger("theInteger", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theInteger), basis::RuntimeException);

   dbms::datatype::LongBlock theLongBlock("theLongBlock", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theLongBlock), basis::RuntimeException);

   dbms::datatype::ShortBlock theShortBlock("theShortBlock", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theShortBlock), basis::RuntimeException);

   dbms::datatype::TimeStamp theTimeStamp("theTimeStamp", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theTimeStamp), basis::RuntimeException);
}

BOOST_AUTO_TEST_CASE(dbms_compare_longblock)
{
   dbms::datatype::LongBlock value("value", dbms::datatype::Constraint::CanBeNull);
   dbms::datatype::LongBlock other("other", dbms::datatype::Constraint::CanBeNull);

   BOOST_REQUIRE_EQUAL(value.hasValue(), false);
   BOOST_REQUIRE_EQUAL(other.hasValue(), false);

   BOOST_REQUIRE_EQUAL(value.compare(other), 0);

   basis::DataBlock datablock;

   datablock.append("hello");
   other.setValue(datablock);

   BOOST_REQUIRE_LT(value.compare(other), 0);
   BOOST_REQUIRE_GT(other.compare(value), 0);

   value.setValue(other.getValue());
   BOOST_REQUIRE_THROW(value.compare(other), basis::RuntimeException);

   datablock = "xabc";
   value.setValue(datablock);
   BOOST_REQUIRE_THROW(value.compare(other), basis::RuntimeException);
   BOOST_REQUIRE_THROW(other.compare(value), basis::RuntimeException);

   other.clear();
   BOOST_REQUIRE_EQUAL(other.hasValue(), false);
   BOOST_REQUIRE_GT(value.compare(other), 0);
   BOOST_REQUIRE_LT(other.compare(value), 0);

   dbms::datatype::Date theDate("theDate", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theDate), basis::RuntimeException);

   dbms::datatype::Float theFloat("theFloat", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theFloat), basis::RuntimeException);

   dbms::datatype::Integer theInteger("theInteger", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theInteger), basis::RuntimeException);

   dbms::datatype::String theString("theString", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theString), basis::RuntimeException);

   dbms::datatype::ShortBlock theShortBlock("theShortBlock", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theShortBlock), basis::RuntimeException);

   dbms::datatype::TimeStamp theTimeStamp("theTimeStamp", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theTimeStamp), basis::RuntimeException);
}

BOOST_AUTO_TEST_CASE(dbms_compare_shortblock)
{
   dbms::datatype::ShortBlock value("value", 16, dbms::datatype::Constraint::CanBeNull);
   dbms::datatype::ShortBlock other("other", 16, dbms::datatype::Constraint::CanBeNull);

   BOOST_REQUIRE_EQUAL(value.hasValue(), false);
   BOOST_REQUIRE_EQUAL(other.hasValue(), false);

   BOOST_REQUIRE_EQUAL(value.compare(other), 0);

   basis::DataBlock datablock;

   datablock.append("hello");
   other.setValue(datablock);

   BOOST_REQUIRE_LT(value.compare(other), 0);
   BOOST_REQUIRE_GT(other.compare(value), 0);

   value.setValue(other.getValue());
   BOOST_REQUIRE_THROW(value.compare(other), basis::RuntimeException);

   datablock = "xabc";
   value.setValue(datablock);
   BOOST_REQUIRE_THROW(value.compare(other), basis::RuntimeException);
   BOOST_REQUIRE_THROW(other.compare(value), basis::RuntimeException);

   other.clear();
   BOOST_REQUIRE_EQUAL(other.hasValue(), false);
   BOOST_REQUIRE_GT(value.compare(other), 0);
   BOOST_REQUIRE_LT(other.compare(value), 0);

   dbms::datatype::Date theDate("theDate", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theDate), basis::RuntimeException);

   dbms::datatype::Float theFloat("theFloat", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theFloat), basis::RuntimeException);

   dbms::datatype::Integer theInteger("theInteger", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theInteger), basis::RuntimeException);

   dbms::datatype::String theString("theString", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theString), basis::RuntimeException);

   dbms::datatype::LongBlock theShortBlock("theShortBlock", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theShortBlock), basis::RuntimeException);

   dbms::datatype::TimeStamp theTimeStamp("theTimeStamp", dbms::datatype::Constraint::CanNotBeNull);
   BOOST_REQUIRE_THROW(value.compare(theTimeStamp), basis::RuntimeException);
}



