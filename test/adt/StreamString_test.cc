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

#include <iostream>

#include <boost/test/unit_test.hpp>

#include <coffee/adt/StreamString.hpp>

using namespace std;
using namespace coffee;

BOOST_AUTO_TEST_CASE( StreamString_integer )
{
   int val = 10;

   adt::StreamString result;

   result = val;
   BOOST_REQUIRE_EQUAL(result, "10");

   result = 0;
   BOOST_REQUIRE_EQUAL(result, "0");

   val = INT_MAX;
   result = val;
   BOOST_REQUIRE_EQUAL(result, "2147483647");

   val = INT_MIN;
   result = val;
   BOOST_REQUIRE_EQUAL(result, "-2147483648");
}

BOOST_AUTO_TEST_CASE( StreamString_uinteger )
{
   unsigned int val = 10;

   adt::StreamString result;

   result = val;
   BOOST_REQUIRE_EQUAL(result, "10");

   result = 0;
   BOOST_REQUIRE_EQUAL(result, "0");

   val = UINT_MAX;
   result = val;
   BOOST_REQUIRE_EQUAL(result, "4294967295");
}

BOOST_AUTO_TEST_CASE( StreamString_integer64 )
{
   Integer64 val = 0;
   adt::StreamString result;

   val --;
   result = val;
   BOOST_REQUIRE_EQUAL(result, "-1");

   val = LLONG_MAX;
   result = val;
   BOOST_REQUIRE_EQUAL(result, "9223372036854775807");

   val = LLONG_MIN;
   result = val;
   BOOST_REQUIRE_EQUAL(result, "-9223372036854775808");
}

BOOST_AUTO_TEST_CASE( StreamString_uinteger64 )
{
   Unsigned64 val = 0;
   adt::StreamString result;

   result = val;
   BOOST_REQUIRE_EQUAL(result, "0");

   val = ULLONG_MAX;
   result = val;
   BOOST_REQUIRE_EQUAL(result, "18446744073709551615");
}

BOOST_AUTO_TEST_CASE( StreamString_bool )
{
   adt::StreamString result;

   result = true;
   BOOST_REQUIRE_EQUAL(result, "true");

   result = false;
   BOOST_REQUIRE_EQUAL(result, "false");
}

BOOST_AUTO_TEST_CASE( StreamString_char_char )
{
   adt::StreamString result;

   result << "hello" << " " << "world";

   BOOST_REQUIRE_EQUAL(result, "hello world");
}

BOOST_AUTO_TEST_CASE( StreamString_char_int )
{
   adt::StreamString result;

   result << "hello" << " " << 3 << "rd world";

   BOOST_REQUIRE_EQUAL(result, "hello 3rd world");
}

BOOST_AUTO_TEST_CASE( StreamString_string_bool )
{
   adt::StreamString result;

   std::string hello ("hello");
   result << hello << " " << true << " world";

   BOOST_REQUIRE_EQUAL(result, "hello true world");
}

BOOST_AUTO_TEST_CASE( StreamString_string_int64 )
{
   adt::StreamString result;

   result << "max_long:" << ULONG_MAX;
   result.toUpper();
   BOOST_REQUIRE_EQUAL(result, "MAX_LONG:18446744073709551615");
}

BOOST_AUTO_TEST_CASE( StreamString_string_null )
{
   adt::StreamString result(adt::StreamString::Flag::ShowNull);

   result << "text=" << (const char*) NULL;
   BOOST_REQUIRE_EQUAL(result, "text=<null>");
}

BOOST_AUTO_TEST_CASE( StreamString_string_double )
{
   adt::StreamString result;

   result << "double=" << (double) 0.000123;
   BOOST_REQUIRE_EQUAL(result, "double=1.230000e-04");
}

BOOST_AUTO_TEST_CASE( StreamString_chrono )
{
   std::chrono::seconds sec (100);
   std::chrono::milliseconds ms(sec);
   std::chrono::microseconds us(sec);

   adt::StreamString result;

   result = sec;
   result << " | " << ms;
   result << " | " << us;
   BOOST_REQUIRE_EQUAL(result, "100 sec | 100000 ms | 100000000 us");
}
