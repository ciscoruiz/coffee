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

#include <wepa/adt/AsString.h>
#include <wepa/adt/DataBlock.h>

using namespace std;
using namespace wepa;

BOOST_AUTO_TEST_CASE( asstring_integer )
{
   int val = 10;

   string result = adt::AsString::apply(val);

   BOOST_REQUIRE_EQUAL(result, "10");

   result = adt::AsString::apply (0);
   BOOST_REQUIRE_EQUAL(result, "0");

   val = INT_MAX;
   result = adt::AsString::apply(val);
   BOOST_REQUIRE_EQUAL(result, "2147483647");

   val = INT_MIN;
   result = adt::AsString::apply(val);
   BOOST_REQUIRE_EQUAL(result, "-2147483648");
}

BOOST_AUTO_TEST_CASE( asstring_integer64 )
{
   Integer64 val = 0;
   string result;

   val --;
   result = adt::AsString::apply(val);
   BOOST_REQUIRE_EQUAL(result, "-1");

   val = LLONG_MAX;
   result = adt::AsString::apply(val);
   BOOST_REQUIRE_EQUAL(result, "9223372036854775807");

   val = LLONG_MIN;
   result = adt::AsString::apply(val);
   BOOST_REQUIRE_EQUAL(result, "-9223372036854775808");
}

BOOST_AUTO_TEST_CASE( asstring_uinteger64 )
{
   Unsigned64 val = 0;
   string result;

   result = adt::AsString::apply(val);
   BOOST_REQUIRE_EQUAL(result, "0");

   val = ULLONG_MAX;
   result = adt::AsString::apply(val);
   BOOST_REQUIRE_EQUAL(result, "18446744073709551615");
}

BOOST_AUTO_TEST_CASE( asstring_bool )
{
   string result;

   result = adt::AsString::apply(true);
   BOOST_REQUIRE_EQUAL(result, "true");

   result = adt::AsString::apply(false);
   BOOST_REQUIRE_EQUAL(result, "false");
}

BOOST_AUTO_TEST_CASE( asstring_datablock )
{
   string result;
   adt::DataBlock value ("hello world", 11);

   result = adt::AsString::apply(value);

   cout << result << endl;

   BOOST_REQUIRE_GT (result.size (), 0);
}
