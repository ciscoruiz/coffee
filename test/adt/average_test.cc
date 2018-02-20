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
