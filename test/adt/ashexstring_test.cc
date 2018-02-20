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

#include <coffee/adt/AsHexString.hpp>
#include <coffee/adt/DataBlock.hpp>

using namespace std;
using namespace coffee;

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
