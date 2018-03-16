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

#include <limits.h>
#include <stdio.h>

#include <iostream>

#include <boost/test/unit_test.hpp>

#include <coffee/adt/AsString.hpp>
#include <coffee/adt/DataBlock.hpp>

using namespace std;
using namespace coffee;

BOOST_AUTO_TEST_CASE( asstring_integer )
{
   int val = 10;

   string result = adt::AsString::apply(val);

   BOOST_REQUIRE_EQUAL(result, "10");

   BOOST_REQUIRE_EQUAL (adt::AsString::apply(val, "%05d"), "00010");

   result = adt::AsString::apply (0);
   BOOST_REQUIRE_EQUAL(result, "0");

   val = INT_MAX;
   result = adt::AsString::apply(val);
   BOOST_REQUIRE_EQUAL(result, "2147483647");

   val = INT_MIN;
   result = adt::AsString::apply(val);
   BOOST_REQUIRE_EQUAL(result, "-2147483648");
}

BOOST_AUTO_TEST_CASE( asstring_uinteger )
{
   unsigned int val = 10;

   string result = adt::AsString::apply(val);

   BOOST_REQUIRE_EQUAL(result, "10");

   result = adt::AsString::apply (0);
   BOOST_REQUIRE_EQUAL(result, "0");

   val = UINT_MAX;
   result = adt::AsString::apply(val);
   BOOST_REQUIRE_EQUAL(result, "4294967295");
}

BOOST_AUTO_TEST_CASE( asstring_integer64 )
{
   int64_t val = 0;
   string result;

   val --;
   result = adt::AsString::apply(val);
   BOOST_REQUIRE_EQUAL(result, "-1");

   val = INT64_MAX;
   result = adt::AsString::apply(val);
   BOOST_REQUIRE_EQUAL(result, "9223372036854775807");

   val = INT64_MIN;
   result = adt::AsString::apply(val);
   BOOST_REQUIRE_EQUAL(result, "-9223372036854775808");
}

BOOST_AUTO_TEST_CASE( asstring_uinteger64 )
{
   uint64_t val = 0;
   string result;

   result = adt::AsString::apply(val);
   BOOST_REQUIRE_EQUAL(result, "0");

   val = UINT64_MAX;
   result = adt::AsString::apply(val);
   BOOST_REQUIRE_EQUAL(result, "18446744073709551615");
}

BOOST_AUTO_TEST_CASE( asstring_bool )
{
   BOOST_REQUIRE_EQUAL(adt::AsString::apply(true), "true");
   BOOST_REQUIRE_EQUAL(adt::AsString::apply(false), "false");
}

BOOST_AUTO_TEST_CASE( asstring_datablock )
{
   char buffer[1024];
   adt::DataBlock value (buffer, 1024);

   const string result = adt::AsString::apply(value);

   char number[5];
   for (int ii = 0; ii <= (1024 - adt::AsString::DefaultCharactersByLine); ii += adt::AsString::DefaultCharactersByLine) {
      sprintf (number, "%4d: ", ii);
      BOOST_REQUIRE(result.find(number) != std::string::npos);
   }

   BOOST_REQUIRE_GT (result.size (), 0);

   value.clear();
   BOOST_REQUIRE_EQUAL(adt::AsString::apply(value), "<null>");
}

BOOST_AUTO_TEST_CASE( asstring_double )
{
   string result = adt::AsString::apply(123.127, "%.02f");
   BOOST_REQUIRE_EQUAL(result, "123.13");
}
