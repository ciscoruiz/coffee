#include <limits.h>

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
   char buffer[1024];
   adt::DataBlock value (buffer, 1024);

   result = adt::AsString::apply(value);

   cout << result << endl;

   BOOST_REQUIRE_GT (result.size (), 0);
}

BOOST_AUTO_TEST_CASE( asstring_double )
{
   string result = adt::AsString::apply(123.127, "%.02f");
   BOOST_REQUIRE_EQUAL(result, "123.13");
}
