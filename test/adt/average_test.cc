// COFFEE - COmpany eFFEEctive Platform
//
// (c) Copyright 2018 Francisco Ruiz Rayo
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#include <boost/test/unit_test.hpp>

#include <coffee/adt/Average.hpp>

#include <limits.h>

using namespace coffee;

BOOST_AUTO_TEST_CASE( avg_basic)
{
   adt::Average <int> average ("basic");

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
   adt::Average <int> average ("integer");

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
   adt::Average <unsigned int> average ("unsigned");

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
