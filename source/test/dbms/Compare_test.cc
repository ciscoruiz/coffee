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


#include <gtest/gtest.h>

#include <coffee/time/TimeService.hpp>

#include <coffee/dbms/datatype/Date.hpp>
#include <coffee/dbms/datatype/Float.hpp>
#include <coffee/dbms/datatype/Integer.hpp>
#include <coffee/dbms/datatype/LongBlock.hpp>
#include <coffee/dbms/datatype/ShortBlock.hpp>
#include <coffee/dbms/datatype/String.hpp>
#include <coffee/dbms/datatype/TimeStamp.hpp>

using namespace coffee;

TEST(CompareDataTypeTest, date)
{
   dbms::datatype::Date value("value", dbms::datatype::Constraint::CanBeNull);
   dbms::datatype::Date other("other", dbms::datatype::Constraint::CanBeNull);

   ASSERT_FALSE(value.hasValue());
   ASSERT_FALSE(other.hasValue());

   ASSERT_EQ(0, value.compare(other));

   auto now = time::TimeService::toSeconds(time::TimeService::now());

   other.setValue(now);

   ASSERT_LT(value.compare(other), 0);
   ASSERT_GT(other.compare(value), 0);

   value.setValue(other.getValue());
   ASSERT_EQ(0, value.compare(other));

   value.setValue(now + std::chrono::seconds(10));
   ASSERT_GT(value.compare(other), 0);
   ASSERT_LT(other.compare(value), 0);

   other.clear();
   ASSERT_TRUE(!other.hasValue());
   ASSERT_GT(value.compare(other), 0);
   ASSERT_LT(other.compare(value), 0);

   dbms::datatype::Float theFloat("theFloat", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theFloat), basis::RuntimeException);

   dbms::datatype::Integer theInteger("theInteger", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theInteger), basis::RuntimeException);

   dbms::datatype::LongBlock theLongBlock("theLongBlock", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theLongBlock), basis::RuntimeException);

   dbms::datatype::ShortBlock theShortBlock("theShortBlock", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theShortBlock), basis::RuntimeException);

   dbms::datatype::String theString("theString", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theString), basis::RuntimeException);

   dbms::datatype::TimeStamp theTimeStamp("theTimeStamp", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theTimeStamp), basis::RuntimeException);
}

TEST(CompareDataTypeTest, float)
{
   dbms::datatype::Float value("value", dbms::datatype::Constraint::CanBeNull);
   dbms::datatype::Float other("other", dbms::datatype::Constraint::CanBeNull);

   ASSERT_EQ(false, value.hasValue());
   ASSERT_EQ(false, other.hasValue());

   ASSERT_EQ(0, value.compare(other));

   other.setValue(0.111);

   ASSERT_LT(value.compare(other), 0);
   ASSERT_GT(other.compare(value), 0);

   value.setValue(other.getValue());
   ASSERT_EQ(0, value.compare(other));

   value.setValue(0.122);
   ASSERT_GT(value.compare(other), 0);

   other.clear();
   ASSERT_EQ(false, other.hasValue());
   ASSERT_GT(value.compare(other), 0);
   ASSERT_LT(other.compare(value), 0);

   dbms::datatype::Date theDate("theDate", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theDate), basis::RuntimeException);

   dbms::datatype::Integer theInteger("theInteger", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theInteger), basis::RuntimeException);

   dbms::datatype::LongBlock theLongBlock("theLongBlock", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theLongBlock), basis::RuntimeException);

   dbms::datatype::ShortBlock theShortBlock("theShortBlock", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theShortBlock), basis::RuntimeException);

   dbms::datatype::String theString("theString", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theString), basis::RuntimeException);

   dbms::datatype::TimeStamp theTimeStamp("theTimeStamp", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theTimeStamp), basis::RuntimeException);
}

TEST(CompareDataTypeTest, float_abstract)
{
   dbms::datatype::Float value("value", dbms::datatype::Constraint::CanBeNull);
   dbms::datatype::Float value2("other", dbms::datatype::Constraint::CanBeNull);

   value.setValue(0.123);
   value2.setValue(0.0);

   dbms::datatype::Abstract& abstract(value2);

   ASSERT_NE(value.compare(abstract), 0);
}

TEST(CompareDataTypeTest, integer)
{
   dbms::datatype::Integer value("value", dbms::datatype::Constraint::CanBeNull);
   dbms::datatype::Integer other("other", dbms::datatype::Constraint::CanBeNull);

   ASSERT_FALSE(value.hasValue());
   ASSERT_FALSE(other.hasValue());

   ASSERT_EQ(0, value.compare(other));

   other.setValue(111);

   ASSERT_LT(value.compare(other), 0);
   ASSERT_GT(other.compare(value), 0);

   value.setValue(other.getValue());
   ASSERT_EQ(0, value.compare(other));

   value.setValue(122);
   ASSERT_GT(value.compare(other), 0);

   other.clear();
   ASSERT_EQ(false, other.hasValue());
   ASSERT_GT(value.compare(other), 0);
   ASSERT_LT(other.compare(value), 0);

   dbms::datatype::Date theDate("theDate", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theDate), basis::RuntimeException);

   dbms::datatype::Float theFloat("theFloat", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theFloat), basis::RuntimeException);

   dbms::datatype::LongBlock theLongBlock("theLongBlock", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theLongBlock), basis::RuntimeException);

   dbms::datatype::ShortBlock theShortBlock("theShortBlock", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theShortBlock), basis::RuntimeException);

   dbms::datatype::String theString("theString", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theString), basis::RuntimeException);

   dbms::datatype::TimeStamp theTimeStamp("theTimeStamp", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theTimeStamp), basis::RuntimeException);
}

TEST(CompareDataTypeTest, string)
{
   dbms::datatype::String value("value", 16, dbms::datatype::Constraint::CanBeNull);
   dbms::datatype::String other("other", 16, dbms::datatype::Constraint::CanBeNull);

   ASSERT_EQ(false, value.hasValue());
   ASSERT_EQ(false, other.hasValue());

   ASSERT_EQ(0, value.compare(other));

   other.setValue("hello");

   ASSERT_LT(value.compare(other), 0);
   ASSERT_GT(other.compare(value), 0);

   value.setValue(other.getValue());
   ASSERT_EQ(0, value.compare(other));

   value.setValue("xabc");
   ASSERT_GT(value.compare(other), 0);
   ASSERT_LT(other.compare(value), 0);

   other.clear();
   ASSERT_EQ(false, other.hasValue());
   ASSERT_GT(value.compare(other), 0);
   ASSERT_LT(other.compare(value), 0);

   dbms::datatype::Date theDate("theDate", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theDate), basis::RuntimeException);

   dbms::datatype::Float theFloat("theFloat", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theFloat), basis::RuntimeException);

   dbms::datatype::Integer theInteger("theInteger", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theInteger), basis::RuntimeException);

   dbms::datatype::LongBlock theLongBlock("theLongBlock", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theLongBlock), basis::RuntimeException);

   dbms::datatype::ShortBlock theShortBlock("theShortBlock", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theShortBlock), basis::RuntimeException);

   dbms::datatype::TimeStamp theTimeStamp("theTimeStamp", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theTimeStamp), basis::RuntimeException);
}

TEST(CompareDataTypeTest, longblock)
{
   dbms::datatype::LongBlock value("value", dbms::datatype::Constraint::CanBeNull);
   dbms::datatype::LongBlock other("other", dbms::datatype::Constraint::CanBeNull);

   ASSERT_EQ(false, value.hasValue());
   ASSERT_EQ(false, other.hasValue());

   ASSERT_EQ(0, value.compare(other));

   basis::DataBlock datablock;

   datablock.append("hello");
   other.setValue(datablock);

   ASSERT_LT(value.compare(other), 0);
   ASSERT_GT(other.compare(value), 0);

   value.setValue(other.getValue());
   ASSERT_THROW(value.compare(other), basis::RuntimeException);

   datablock = "xabc";
   value.setValue(datablock);
   ASSERT_THROW(value.compare(other), basis::RuntimeException);
   ASSERT_THROW(other.compare(value), basis::RuntimeException);

   other.clear();
   ASSERT_EQ(false, other.hasValue());
   ASSERT_GT(value.compare(other), 0);
   ASSERT_LT(other.compare(value), 0);

   dbms::datatype::Date theDate("theDate", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theDate), basis::RuntimeException);

   dbms::datatype::Float theFloat("theFloat", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theFloat), basis::RuntimeException);

   dbms::datatype::Integer theInteger("theInteger", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theInteger), basis::RuntimeException);

   dbms::datatype::String theString("theString", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theString), basis::RuntimeException);

   dbms::datatype::ShortBlock theShortBlock("theShortBlock", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theShortBlock), basis::RuntimeException);

   dbms::datatype::TimeStamp theTimeStamp("theTimeStamp", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theTimeStamp), basis::RuntimeException);
}

TEST(CompareDataTypeTest, shortblock)
{
   dbms::datatype::ShortBlock value("value", 16, dbms::datatype::Constraint::CanBeNull);
   dbms::datatype::ShortBlock other("other", 16, dbms::datatype::Constraint::CanBeNull);

   ASSERT_EQ(false, value.hasValue());
   ASSERT_EQ(false, other.hasValue());

   ASSERT_EQ(0, value.compare(other));

   basis::DataBlock datablock;

   datablock.append("hello");
   other.setValue(datablock);

   ASSERT_LT(value.compare(other), 0);
   ASSERT_GT(other.compare(value), 0);

   value.setValue(other.getValue());
   ASSERT_THROW(value.compare(other), basis::RuntimeException);

   datablock = "xabc";
   value.setValue(datablock);
   ASSERT_THROW(value.compare(other), basis::RuntimeException);
   ASSERT_THROW(other.compare(value), basis::RuntimeException);

   other.clear();
   ASSERT_EQ(false, other.hasValue());
   ASSERT_GT(value.compare(other), 0);
   ASSERT_LT(other.compare(value), 0);

   dbms::datatype::Date theDate("theDate", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theDate), basis::RuntimeException);

   dbms::datatype::Float theFloat("theFloat", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theFloat), basis::RuntimeException);

   dbms::datatype::Integer theInteger("theInteger", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theInteger), basis::RuntimeException);

   dbms::datatype::String theString("theString", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theString), basis::RuntimeException);

   dbms::datatype::LongBlock theShortBlock("theShortBlock", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theShortBlock), basis::RuntimeException);

   dbms::datatype::TimeStamp theTimeStamp("theTimeStamp", dbms::datatype::Constraint::CanNotBeNull);
   ASSERT_THROW(value.compare(theTimeStamp), basis::RuntimeException);
}



