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

#include <coffee/dbms/datatype/TimeStamp.hpp>
#include <coffee/dbms/datatype/Integer.hpp>

#include "PrintChrono.hpp"

using namespace coffee;
using namespace coffee::dbms;

BOOST_AUTO_TEST_CASE(timestamp_is_not_nulleable)
{
   datatype::TimeStamp column("not_nulleable", datatype::Constraint::CanNotBeNull);

   BOOST_REQUIRE(column.hasValue());

   BOOST_REQUIRE_THROW(column.isNull(), adt::RuntimeException);

   column.clear();
   BOOST_REQUIRE(column.hasValue());
   BOOST_REQUIRE_EQUAL(column.getValue(), std::chrono::seconds::zero());
}

BOOST_AUTO_TEST_CASE(timestamp_downcast)
{
   datatype::TimeStamp column("not_nulleable", datatype::Constraint::CanNotBeNull);

   datatype::Abstract& abs(column);

   BOOST_REQUIRE(abs.hasValue());

   const datatype::TimeStamp& other = coffee_datatype_downcast(datatype::TimeStamp, abs);

   const char* format = "%d/%m/%YT%H:%M:%S";
   std::string str_TimeStamp("01/01/1920T12:30:50");
   column.setValue(str_TimeStamp, format);

   BOOST_REQUIRE(other == column);

   datatype::Integer zzz("zzz");

   BOOST_REQUIRE_THROW(coffee_datatype_downcast(datatype::TimeStamp, zzz), dbms::InvalidDataException);
}
