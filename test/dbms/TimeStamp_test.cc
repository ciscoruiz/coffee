// COFFEE - COmpany eFFEEctive Platform
//
//(c) Copyright 2018 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/coffee
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#include <iostream>
#include <time.h>

#include <boost/test/unit_test.hpp>

#include <coffee/dbms/datatype/TimeStamp.hpp>
#include <coffee/dbms/datatype/Integer.hpp>

using namespace coffee;
using namespace coffee::dbms;


// See http://www.mbari.org/staff/rich/utccalc.htm
BOOST_AUTO_TEST_CASE(timestamp_setter_second)
{
   adt::Second timestamp_01_31_1987_23_59_59(539110799);
   datatype::TimeStamp column("from_number");

   column.setValue(timestamp_01_31_1987_23_59_59);

   tm* localTime = column.getLocalTime();

   BOOST_REQUIRE_EQUAL(localTime->tm_mday, 31);
   BOOST_REQUIRE_EQUAL(localTime->tm_mon, 0);
   BOOST_REQUIRE_EQUAL(localTime->tm_year, 1987 - 1900);
   BOOST_REQUIRE_EQUAL(localTime->tm_min, 59);
   BOOST_REQUIRE_EQUAL(localTime->tm_sec, 59);

   BOOST_REQUIRE_EQUAL(column.getValue(), 539110799);
}

BOOST_AUTO_TEST_CASE(timestamp_setter_text)
{
   datatype::TimeStamp column("from_text");

   std::string str_TimeStamp("31/01/1996T22:17:10");

   try {
      column.setValue(str_TimeStamp, "%d/%m/%YT%T");

      tm* localTime = column.getLocalTime();

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

BOOST_AUTO_TEST_CASE(timestamp_is_nulleable)
{
   datatype::TimeStamp column("timestamp_is_nulleable", datatype::Constraint::CanBeNull);

   const char* format = "%d/%m/%YT%H:%M:%S";

   BOOST_REQUIRE_EQUAL(column.hasValue(), false);

   column.clear();
   BOOST_REQUIRE_EQUAL(column.hasValue(), false);

   std::string str_TimeStamp("01/01/1800T12:30:50");
   column.setValue(str_TimeStamp, format);
   BOOST_REQUIRE_EQUAL(column.hasValue(), true);

   BOOST_REQUIRE_NO_THROW(column.setValue("01/01/2000T00:00:00", format));
   BOOST_REQUIRE_EQUAL(column.hasValue(), true);

   column.clear();
   BOOST_REQUIRE_THROW(column.getValue(), adt::RuntimeException);
   BOOST_REQUIRE_THROW(column.getLocalTime(), adt::RuntimeException);

   str_TimeStamp = "25/10/2013T02:00:10";

   column.setValue(str_TimeStamp, "%d/%m/%YT%H:%M:%S");
   BOOST_REQUIRE_EQUAL(column.hasValue(), true);

   const tm* time_t = column.getLocalTime();
   BOOST_REQUIRE_EQUAL(time_t->tm_year, 2013 - 1900);
   BOOST_REQUIRE_EQUAL(time_t->tm_mon, 9);
   BOOST_REQUIRE_EQUAL(time_t->tm_mday, 25);

   column.clear();
   BOOST_REQUIRE_EQUAL(column.hasValue(), false);
}

BOOST_AUTO_TEST_CASE(timestamp_is_not_nulleable)
{
   datatype::TimeStamp column("not_nulleable", datatype::Constraint::CanNotBeNull);

   BOOST_REQUIRE_EQUAL(column.hasValue(), true);

   BOOST_REQUIRE_THROW(column.isNull(), adt::RuntimeException);

   column.clear();
   BOOST_REQUIRE_EQUAL(column.hasValue(), true);

   BOOST_REQUIRE_EQUAL(column.getValue(), 0);
}

BOOST_AUTO_TEST_CASE(timestamp_downcast)
{
   datatype::TimeStamp column("not_nulleable", datatype::Constraint::CanNotBeNull);

   datatype::Abstract& abs = column;

   BOOST_REQUIRE_EQUAL(abs.hasValue(), true);

   const datatype::TimeStamp& other = coffee_datatype_downcast(datatype::TimeStamp, abs);

   const char* format = "%d/%m/%YT%H:%M:%S";
   std::string str_TimeStamp("01/01/1800T12:30:50");
   column.setValue(str_TimeStamp, format);

   BOOST_REQUIRE_EQUAL(other == column, true);

   datatype::Integer zzz("zzz");

   BOOST_REQUIRE_THROW(coffee_datatype_downcast(datatype::TimeStamp, zzz), dbms::InvalidDataException);
}

BOOST_AUTO_TEST_CASE(timestamp_clone)
{
   datatype::TimeStamp cannotBeNull("cannotBeNull", datatype::Constraint::CanNotBeNull);
   datatype::TimeStamp canBeNull("canBeNull", datatype::Constraint::CanBeNull);

   BOOST_REQUIRE_EQUAL(cannotBeNull.hasValue(), true);
   BOOST_REQUIRE_EQUAL(canBeNull.hasValue(), false);

   std::shared_ptr<datatype::Abstract> notnull(cannotBeNull.clone());
   std::shared_ptr<datatype::Abstract> null(canBeNull.clone());

   BOOST_REQUIRE_EQUAL(notnull->hasValue(), true);
   BOOST_REQUIRE_EQUAL(null->hasValue(), false);

   BOOST_REQUIRE_EQUAL(notnull->compare(cannotBeNull), 0);

   cannotBeNull.setValue(adt::Second(5));

   BOOST_REQUIRE_EQUAL(cannotBeNull.getValue(), adt::Second(5));

   notnull = cannotBeNull.clone();
   BOOST_REQUIRE_EQUAL(notnull->hasValue(), true);
   BOOST_REQUIRE_EQUAL(notnull->compare(cannotBeNull), 0);

   canBeNull.setValue(adt::Second(25));
   null = canBeNull.clone();
   BOOST_REQUIRE_EQUAL(null->hasValue(), true);
   BOOST_REQUIRE_EQUAL(null->compare(canBeNull), 0);

   BOOST_REQUIRE_EQUAL(null->compare(cannotBeNull), 20);

   BOOST_REQUIRE_EQUAL(notnull->compare(canBeNull), -20);
}

BOOST_AUTO_TEST_CASE(timestamp_fractional)
{
   datatype::TimeStamp data("timestamp");
   adt::Second now = adt::Second::getLocalTime();
   data.setValue(now);
   data.setFractionalSecond(100);

   datatype::TimeStamp copy(data);
   BOOST_REQUIRE(copy.getValue() == now);
   BOOST_REQUIRE(copy.getFractionalSecond() == 100);
}
