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

#include <boost/test/unit_test.hpp>

#include <coffee/basis/Average.hpp>

#include <limits.h>

using namespace coffee;

BOOST_AUTO_TEST_CASE( avg_basic)
{
   basis::Average <int> average ("basic");

   BOOST_REQUIRE_EQUAL(average.isEmpty(), true);
   BOOST_REQUIRE_EQUAL(average.isZero (), true);

   average = 10;
   average = 5;
   average += 7;

   BOOST_REQUIRE_EQUAL(average.value (), 6);
   BOOST_REQUIRE_EQUAL(average, 6);

   BOOST_REQUIRE_EQUAL(average.isEmpty(), false);
   BOOST_REQUIRE_EQUAL(average.isZero (), false);
   BOOST_REQUIRE_EQUAL(average.size (), 2);

   BOOST_REQUIRE_EQUAL(average.getAccumulator(), 12);

   average.clear ();
   BOOST_REQUIRE_EQUAL(average.isEmpty(), true);
   BOOST_REQUIRE_EQUAL(average.isZero (), true);
   BOOST_REQUIRE_EQUAL(average.value (), 0);
   BOOST_REQUIRE_EQUAL(average, 0);
}

BOOST_AUTO_TEST_CASE( signed_overflow )
{
   basis::Average <int> average ("integer");

   const int someValue = 157234;

   int nloop = INT_MAX / someValue;

   for (int ii = 0; ii < nloop; ++ ii) {
      average += someValue;
   }

   BOOST_REQUIRE_EQUAL(average.size (), nloop);
   BOOST_REQUIRE_EQUAL(average.value (), someValue);

   int overflow = INT_MAX - average.getAccumulator();

   BOOST_REQUIRE_GT(overflow, 0);

   average += (overflow + 1);

   BOOST_REQUIRE_EQUAL(average.size (), 1);
   BOOST_REQUIRE_EQUAL(average.value (), overflow + 1);

}

BOOST_AUTO_TEST_CASE( unsigned_overflow )
{
   basis::Average <unsigned int> average ("unsigned");

   const unsigned int someValue = 157234;

   int nloop = UINT_MAX / someValue;

   for (int ii = 0; ii < nloop; ++ ii) {
      average += someValue;
   }

   BOOST_REQUIRE_EQUAL(average.size (), nloop);
   BOOST_REQUIRE_EQUAL(average.value (), someValue);

   unsigned int overflow = UINT_MAX - average.getAccumulator();

   BOOST_REQUIRE_GT(overflow, 0);

   average += (overflow + 1);

   BOOST_REQUIRE_EQUAL(average.size (), 1);
   BOOST_REQUIRE_EQUAL(average.value (), overflow + 1);

}
