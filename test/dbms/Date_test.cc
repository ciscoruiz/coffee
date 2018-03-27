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

#include <boost/test/unit_test.hpp>

#include <coffee/adt/AsString.hpp>

#include <coffee/dbms/datatype/Date.hpp>
#include <coffee/dbms/datatype/Integer.hpp>

#include <coffee/time/TimeService.hpp>

#include "PrintChrono.hpp"

using namespace coffee;
using namespace coffee::dbms;

struct tm* getLocalTime(const std::chrono::seconds& seconds)
   throw (adt::RuntimeException)
{
   time_t tt = seconds.count();
   tm* result = localtime ((time_t*) &tt);

   if (result == NULL) {
      COFFEE_THROW_EXCEPTION(seconds.count() << " | It is not a valid date");
   }

   return result;
}

// See http://www.mbari.org/staff/rich/utccalc.htm
BOOST_AUTO_TEST_CASE(date_setter_second)
{
   std::chrono::seconds date_01_31_1987_23_59_59(539110799);
   datatype::Date column("from_number");

   column.setValue(date_01_31_1987_23_59_59);

   BOOST_REQUIRE_EQUAL(column.getValue().count(), 539110799);
}

#include <iostream>

BOOST_AUTO_TEST_CASE(date_setter_now)
{
   auto now = coffee::time::TimeService::toSeconds(coffee::time::TimeService::now());

   datatype::Date column("from_now");
   column.setValue(now);

   const std::string text = adt::AsString::apply(column.getValue(), datatype::Date::DefaultFormat);

   datatype::Date other("from_text");
   other.setValue(text, datatype::Date::DefaultFormat);

   BOOST_REQUIRE_EQUAL(column.getValue(), now);
   BOOST_REQUIRE_EQUAL(column.getValue(), other.getValue());
}

BOOST_AUTO_TEST_CASE(date_setter_text)
{
   datatype::Date column("from_text");

   std::string str_date("31/01/1996T22:17:10");

   try {
      column.setValue(str_date, "%d/%m/%YT%T");

      tm* localTime = getLocalTime(column.getValue());

      BOOST_REQUIRE_EQUAL(localTime->tm_mday, 31);
      BOOST_REQUIRE_EQUAL(localTime->tm_mon, 0);
      BOOST_REQUIRE_EQUAL(localTime->tm_year, 1996 - 1900);
      BOOST_REQUIRE_EQUAL(localTime->tm_hour, 22); // This test was written under GMT+1
      BOOST_REQUIRE_EQUAL(localTime->tm_min, 17);
      BOOST_REQUIRE_EQUAL(localTime->tm_sec, 10);
   }
   catch(adt::RuntimeException& ex) {
      std::cout << ex.what() << std::endl;
   }

   BOOST_REQUIRE_THROW(column.setValue("12:10", "%d/%m/%YT%T"), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(date_is_nulleable)
{
   datatype::Date column("date_is_nulleable", datatype::Constraint::CanBeNull);

   const char* format = "%d/%m/%YT%H:%M:%S";

   BOOST_REQUIRE(!column.hasValue());

   column.clear();
   BOOST_REQUIRE(!column.hasValue());

   std::string str_date("01/01/1970T12:30:50");
   BOOST_REQUIRE_NO_THROW(column.setValue(str_date, format));
   BOOST_REQUIRE(column.hasValue());

   BOOST_REQUIRE_NO_THROW(column.setValue("01/01/2000T00:00:00", format));
   BOOST_REQUIRE(column.hasValue());

   column.clear();
   BOOST_REQUIRE_THROW(column.getValue(), adt::RuntimeException);

   str_date = "25/10/2013T02:00:10";

   column.setValue(str_date, "%d/%m/%YT%H:%M:%S");
   BOOST_REQUIRE(column.hasValue());

   const tm* time_t = getLocalTime(column.getValue());
   BOOST_REQUIRE_EQUAL(time_t->tm_year, 2013 - 1900);
   BOOST_REQUIRE_EQUAL(time_t->tm_mon, 9);
   BOOST_REQUIRE_EQUAL(time_t->tm_mday, 25);

   column.clear();
   BOOST_REQUIRE(!column.hasValue());
}

BOOST_AUTO_TEST_CASE(date_is_not_nulleable)
{
   datatype::Date column("not_nulleable", datatype::Constraint::CanNotBeNull);

   BOOST_REQUIRE(column.hasValue());

   BOOST_REQUIRE_THROW(column.isNull(), adt::RuntimeException);

   column.clear();
   BOOST_REQUIRE(column.hasValue());

   BOOST_REQUIRE(column.getValue() == std::chrono::seconds::zero());
}

BOOST_AUTO_TEST_CASE(date_downcast)
{
   datatype::Date column("not_nulleable", datatype::Constraint::CanNotBeNull);

   datatype::Abstract& abs(column);

   BOOST_REQUIRE(abs.hasValue());

   const datatype::Date& other = coffee_datatype_downcast(datatype::Date, abs);

   const char* format = "%d/%m/%YT%H:%M:%S";
   std::string str_date("01/01/1950T12:30:50");
   column.setValue(str_date, format);

   BOOST_REQUIRE(other == column);

   datatype::Integer zzz("zzz");

   BOOST_REQUIRE_THROW(coffee_datatype_downcast(datatype::Date, zzz), dbms::InvalidDataException);
}

BOOST_AUTO_TEST_CASE(date_clone)
{
   datatype::Date cannotBeNull("cannotBeNull", datatype::Constraint::CanNotBeNull);
   datatype::Date canBeNull("canBeNull", datatype::Constraint::CanBeNull);

   BOOST_REQUIRE(cannotBeNull.hasValue());
   BOOST_REQUIRE(!canBeNull.hasValue());

   std::shared_ptr<datatype::Abstract> notnull(cannotBeNull.clone());
   std::shared_ptr<datatype::Abstract> null(canBeNull.clone());

   BOOST_REQUIRE(notnull->hasValue());
   BOOST_REQUIRE(!null->hasValue());

   BOOST_REQUIRE_EQUAL(notnull->compare(cannotBeNull), 0);

   const std::chrono::seconds fiveSeconds(5);

   cannotBeNull.setValue(fiveSeconds);

   BOOST_REQUIRE_EQUAL(cannotBeNull.getValue(), fiveSeconds);

   notnull = cannotBeNull.clone();
   BOOST_REQUIRE(notnull->hasValue());
   BOOST_REQUIRE_EQUAL(notnull->compare(cannotBeNull), 0);
}

BOOST_AUTO_TEST_CASE(date_clone_nuleable)
{
   datatype::Date canBeNull("canBeNull", datatype::Constraint::CanBeNull);
   datatype::Date cannotBeNull("cannotBeNull", datatype::Constraint::CanNotBeNull);

   cannotBeNull.setValue(std::chrono::seconds(5));
   canBeNull.setValue(std::chrono::seconds(25));

   auto null = canBeNull.clone();
   BOOST_REQUIRE(null->hasValue());
   BOOST_REQUIRE_EQUAL(null->compare(canBeNull), 0);

   BOOST_REQUIRE_EQUAL(null->compare(cannotBeNull), 20);
   BOOST_REQUIRE_EQUAL(cannotBeNull.compare(null), -20);
}

