#define BOOST_TEST_MODULE COFFEE_CONFIG_TEST

#include <boost/test/included/unit_test.hpp>

#include <iostream>

#include <limits.h>

#include <coffee/config/Release.hpp>
#include <coffee/config/defines.hpp>

using namespace std;
using namespace coffee;

BOOST_AUTO_TEST_CASE (release)
{
   string version = config::Release::getVersion();

   cout << version << endl;

   BOOST_REQUIRE (version.empty() == false);

   int debug = version.find("/D");
   int release = version.find("/O");

   #ifdef _DEBUG
      BOOST_REQUIRE (debug != string::npos);
      BOOST_REQUIRE (release == string::npos);
   #else
      BOOST_REQUIRE (debug == string::npos);
      BOOST_REQUIRE (release != string::npos);
   #endif
}

BOOST_AUTO_TEST_CASE (numbers)
{
   Integer64 ii64;

   ii64 = LLONG_MAX;
   BOOST_REQUIRE_EQUAL (ii64, LLONG_MAX);

   ii64 = LLONG_MIN;
   BOOST_REQUIRE_EQUAL (ii64, LLONG_MIN);

   Unsigned64 u64;
   u64 = ULLONG_MAX;
   BOOST_REQUIRE_EQUAL (u64, ULLONG_MAX);
}
