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

#include <iostream>
#include <time.h>

#include <gtest/gtest.h>

#include <coffee/basis/AsString.hpp>

#include <coffee/dbms/datatype/Date.hpp>
#include <coffee/dbms/datatype/Integer.hpp>

#include <coffee/time/TimeService.hpp>

using namespace coffee;
using namespace coffee::dbms;

struct tm* getLocalTime(const std::chrono::seconds& seconds)
   throw (basis::RuntimeException)
{
   time_t tt = seconds.count();
   tm* result = localtime ((time_t*) &tt);

   if (result == NULL) {
      COFFEE_THROW_EXCEPTION(seconds.count() << " | It is not a valid date");
   }

   return result;
}

// See http://www.mbari.org/staff/rich/utccalc.htm
TEST(DateTest, setter_second)
{
   std::chrono::seconds date_01_31_1987_23_59_59(539110799);
   datatype::Date column("from_number");

   column.setValue(date_01_31_1987_23_59_59);

   ASSERT_EQ(539110799, column.getValue().count());
}

TEST(DateTest, setter_now)
{
   auto now = coffee::time::TimeService::toSeconds(coffee::time::TimeService::now());

   datatype::Date column("from_now");
   column.setValue(now);

   const std::string text = basis::AsString::apply(column.getValue(), datatype::Date::DefaultFormat);

   datatype::Date other("from_text");
   other.setValue(text, datatype::Date::DefaultFormat);

   ASSERT_EQ(now, column.getValue());
   ASSERT_EQ(other.getValue(), column.getValue());
}

TEST(DateTest, setter_text)
{
   datatype::Date column("from_text");

   std::string str_date("31/01/1996T22:17:10");

   try {
      column.setValue(str_date, "%d/%m/%YT%T");

      tm* localTime = getLocalTime(column.getValue());

      ASSERT_EQ(31, localTime->tm_mday);
      ASSERT_EQ(0, localTime->tm_mon);
      ASSERT_EQ(1996 - 1900, localTime->tm_year);
      ASSERT_EQ(22, localTime->tm_hour); // This test was written under GMT+1
      ASSERT_EQ(17, localTime->tm_min);
      ASSERT_EQ(10, localTime->tm_sec);
   }
   catch(basis::RuntimeException& ex) {
      std::cout << ex.what() << std::endl;
   }

   ASSERT_THROW(column.setValue("12:10", "%d/%m/%YT%T"), basis::RuntimeException);
}

TEST(DateTest, is_nulleable)
{
   datatype::Date column("date_is_nulleable", datatype::Constraint::CanBeNull);

   const char* format = "%d/%m/%YT%H:%M:%S";

   ASSERT_TRUE(!column.hasValue());

   column.clear();
   ASSERT_TRUE(!column.hasValue());

   std::string str_date("01/01/1970T12:30:50");
   ASSERT_NO_THROW(column.setValue(str_date, format));
   ASSERT_TRUE(column.hasValue());

   ASSERT_NO_THROW(column.setValue("01/01/2000T00:00:00", format));
   ASSERT_TRUE(column.hasValue());

   column.clear();
   ASSERT_THROW(column.getValue(), basis::RuntimeException);

   str_date = "25/10/2013T02:00:10";

   column.setValue(str_date, "%d/%m/%YT%H:%M:%S");
   ASSERT_TRUE(column.hasValue());

   const tm* time_t = getLocalTime(column.getValue());
   ASSERT_EQ(2013 - 1900, time_t->tm_year);
   ASSERT_EQ(9, time_t->tm_mon);
   ASSERT_EQ(25, time_t->tm_mday);

   column.clear();
   ASSERT_TRUE(!column.hasValue());
}

TEST(DateTest, is_not_nulleable)
{
   datatype::Date column("not_nulleable", datatype::Constraint::CanNotBeNull);

   ASSERT_TRUE(column.hasValue());

   ASSERT_THROW(column.isNull(), basis::RuntimeException);

   column.clear();
   ASSERT_TRUE(column.hasValue());

   ASSERT_TRUE(column.getValue() == std::chrono::seconds::zero());
}

TEST(DateTest, downcast)
{
   datatype::Date column("not_nulleable", datatype::Constraint::CanNotBeNull);

   datatype::Abstract& abs(column);

   ASSERT_TRUE(abs.hasValue());

   const datatype::Date& other = coffee_datatype_downcast(datatype::Date, abs);

   const char* format = "%d/%m/%YT%H:%M:%S";
   std::string str_date("01/01/1950T12:30:50");
   column.setValue(str_date, format);

   ASSERT_TRUE(other == column);

   datatype::Integer zzz("zzz");

   ASSERT_THROW(coffee_datatype_downcast(datatype::Date, zzz), dbms::InvalidDataException);
}

TEST(DateTest, clone)
{
   datatype::Date cannotBeNull("cannotBeNull", datatype::Constraint::CanNotBeNull);
   datatype::Date canBeNull("canBeNull", datatype::Constraint::CanBeNull);

   ASSERT_TRUE(cannotBeNull.hasValue());
   ASSERT_TRUE(!canBeNull.hasValue());

   std::shared_ptr<datatype::Abstract> notnull(cannotBeNull.clone());
   std::shared_ptr<datatype::Abstract> null(canBeNull.clone());

   ASSERT_TRUE(notnull->hasValue());
   ASSERT_TRUE(!null->hasValue());

   ASSERT_EQ(0, notnull->compare(cannotBeNull));

   const std::chrono::seconds fiveSeconds(5);

   cannotBeNull.setValue(fiveSeconds);

   ASSERT_EQ(fiveSeconds, cannotBeNull.getValue());

   notnull = cannotBeNull.clone();
   ASSERT_TRUE(notnull->hasValue());
   ASSERT_EQ(0, notnull->compare(cannotBeNull));
}

TEST(DateTest, clone_nuleable)
{
   datatype::Date canBeNull("canBeNull", datatype::Constraint::CanBeNull);
   datatype::Date cannotBeNull("cannotBeNull", datatype::Constraint::CanNotBeNull);

   cannotBeNull.setValue(std::chrono::seconds(5));
   canBeNull.setValue(std::chrono::seconds(25));

   auto null = canBeNull.clone();
   ASSERT_TRUE(null->hasValue());
   ASSERT_EQ(0, null->compare(canBeNull));

   ASSERT_EQ(20, null->compare(cannotBeNull));
   ASSERT_EQ(-20, cannotBeNull.compare(null));
}

TEST(DateTest, instantiate) {
   auto data = datatype::Date::instantiate("nulleable");
   ASSERT_TRUE(data->hasValue());

   data = datatype::Date::instantiate("not-nulleable", datatype::Constraint::CanBeNull);
   ASSERT_TRUE(!data->hasValue());
}
