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
#include <limits.h>

#include <iostream>

#include <boost/test/unit_test.hpp>

#include <wepa/adt/StreamString.hpp>

using namespace std;
using namespace wepa;

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

   result << "max_int:" << ULLONG_MAX;
   result.toUpper();
   BOOST_REQUIRE_EQUAL(result, "MAX_INT:18446744073709551615");
}

