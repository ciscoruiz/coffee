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
