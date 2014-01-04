// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2013 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/wepa
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#include <iostream>
#include <time.h>

#include <boost/test/unit_test.hpp>

#include <wepa/dbms/datatype/Date.hpp>

using namespace wepa;
using namespace wepa::dbms;


// See http://www.mbari.org/staff/rich/utccalc.htm
BOOST_AUTO_TEST_CASE (date_setter_second)
{
   adt::Second date_01_31_1987_23_59_59 (539110799);
   datatype::Date column("from_number");

   column.setValue(date_01_31_1987_23_59_59);

   BOOST_REQUIRE_EQUAL(column.getDay(), 31);
   BOOST_REQUIRE_EQUAL(column.getMonth(), 1);
   BOOST_REQUIRE_EQUAL(column.getYear (), 1987);
   BOOST_REQUIRE_EQUAL(column.getHour(), 17); // This test was written under GMT+1
   BOOST_REQUIRE_EQUAL(column.getMinute(), 59);
   BOOST_REQUIRE_EQUAL(column.getSecond (), 59);

   BOOST_REQUIRE_EQUAL (column.getSecondValue (), 539110799);

   BOOST_REQUIRE_EQUAL (strcmp ("31/01/1987 17:59:59", column.getCStringValue()), 0);
}

BOOST_AUTO_TEST_CASE (date_setter_text)
{
   datatype::Date column ("from_text", false, "%d/%m/%YT%T");

   std::string str_date ("31/01/1996T22:17:10");

   try {
      column.setValue (str_date);

      BOOST_REQUIRE_EQUAL(column.getDay(), 31);
      BOOST_REQUIRE_EQUAL(column.getMonth(), 1);
      BOOST_REQUIRE_EQUAL(column.getYear (), 1996);
      BOOST_REQUIRE_EQUAL(column.getHour(), 22);
      BOOST_REQUIRE_EQUAL(column.getMinute(), 17);
   }
   catch (adt::RuntimeException& ex) {
      std::cout << ex.what () << std::endl;
   }

   BOOST_REQUIRE_EQUAL (str_date, column.getCStringValue());

   BOOST_REQUIRE_THROW (column.setValue ("12:10"), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE (date_is_nulleable)
{
   datatype::Date column ("nulleable", true, "%d/%m/%YT%H:%M");

   BOOST_REQUIRE_EQUAL (column.isNull(), true);

   column.setNull(false);

   BOOST_REQUIRE_EQUAL (column.isNull(), false);
   std::string str_date ("00/01/1900T00:00");
   BOOST_REQUIRE_EQUAL (str_date, column.getCStringValue());

   column.setNull(true);
   BOOST_REQUIRE_THROW (column.getValue (), adt::RuntimeException);
   BOOST_REQUIRE_THROW (column.getCStringValue (), adt::RuntimeException);
   BOOST_REQUIRE_THROW (column.getSecondValue (), adt::RuntimeException);

   str_date = "25/10/2013T02:00";

   column.setValue(str_date);
   BOOST_REQUIRE_EQUAL (column.isNull(), false);

   const tm& time_t = column.getValue();
   BOOST_REQUIRE_EQUAL (time_t.tm_year, 2013 - 1900);
   BOOST_REQUIRE_EQUAL (time_t.tm_mon, 9);
   BOOST_REQUIRE_EQUAL (time_t.tm_mday, 25);

   try {
      BOOST_REQUIRE_EQUAL (column.getSecondValue(), adt::Second (1382659200));
   }
   catch (adt::RuntimeException& ex) {
      std::cout << ex.asString () << std::endl;
      BOOST_REQUIRE_EQUAL(0, 1);
   }

   BOOST_REQUIRE_EQUAL (str_date, column.getCStringValue());

   column.clear ();
   BOOST_REQUIRE_EQUAL (column.isNull(), true);
}

BOOST_AUTO_TEST_CASE (date_is_not_nulleable)
{
   datatype::Date column ("not_nulleable", false, "%d/%m/%YT%H:%M");

   BOOST_REQUIRE_EQUAL (column.isNull(), false);

   BOOST_REQUIRE_THROW (column.setNull (true), adt::RuntimeException);

   column.clear();
   BOOST_REQUIRE_EQUAL (column.isNull(), false);

   std::string str_date ("00/01/1900T00:00");
   BOOST_REQUIRE_EQUAL (str_date, column.getCStringValue());
}
