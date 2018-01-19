// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2018 Francisco Ruiz Rayo
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

#include <wepa/adt/AsHexString.hpp>
#include <wepa/adt/DataBlock.hpp>

using namespace std;
using namespace wepa;

BOOST_AUTO_TEST_CASE( ashexstring_integer )
{
   int val = 10;

   string result = adt::AsHexString::apply(val);

   BOOST_REQUIRE_EQUAL(result, "0xa");

   result = adt::AsHexString::apply (0);
   BOOST_REQUIRE_EQUAL(result, "0x0");

   val = INT_MAX;
   result = adt::AsHexString::apply(val);
   BOOST_REQUIRE_EQUAL(result, "0x7fffffff");

   val = INT_MIN;
   result = adt::AsHexString::apply(val);
   BOOST_REQUIRE_EQUAL(result, "0x80000000");
}

BOOST_AUTO_TEST_CASE( ashexstring_uinteger )
{
   unsigned int val = 10;

   string result = adt::AsHexString::apply(val);

   BOOST_REQUIRE_EQUAL(result, "0xa");

   result = adt::AsHexString::apply (0);
   BOOST_REQUIRE_EQUAL(result, "0x0");

   val = UINT_MAX;
   result = adt::AsHexString::apply(val);
   BOOST_REQUIRE_EQUAL(result, "0xffffffff");
}

BOOST_AUTO_TEST_CASE( ashexstring_integer64 )
{
   Integer64 val = 0;
   string result;

   val --;
   result = adt::AsHexString::apply(val);
   BOOST_REQUIRE_EQUAL(result, "0xffffffffffffffff");

   val = LLONG_MAX;
   result = adt::AsHexString::apply(val);
   BOOST_REQUIRE_EQUAL(result, "0x7fffffffffffffff");

   val = LLONG_MIN;
   result = adt::AsHexString::apply(val);
   BOOST_REQUIRE_EQUAL(result, "0x8000000000000000");
}

BOOST_AUTO_TEST_CASE( ashexstring_uinteger64 )
{
   Unsigned64 val = 0;
   string result;

   result = adt::AsHexString::apply(val);
   BOOST_REQUIRE_EQUAL(result, "0x0");

   val = ULLONG_MAX;
   result = adt::AsHexString::apply(val);
   BOOST_REQUIRE_EQUAL(result, "0xffffffffffffffff");
}
