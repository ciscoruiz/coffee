#include <boost/test/unit_test.hpp>

#include <wepa/adt/DataBlock.h>
#include <wepa/adt/RuntimeException.h>

using namespace std;
using namespace wepa;

BOOST_AUTO_TEST_CASE( datablock_size )
{
   adt::DataBlock var ("hello xxx", 5);

   BOOST_REQUIRE_EQUAL (var.size (), 5);

   var.assign("second");

   BOOST_REQUIRE_EQUAL (var.size (), 6);
}

BOOST_AUTO_TEST_CASE( datablock_copy )
{
   const char* pp = "hello xxx";

   adt::DataBlock var (pp, strlen (pp));

   BOOST_REQUIRE_EQUAL (var.size (), strlen (pp));

   adt::DataBlock other (var);

   BOOST_REQUIRE_EQUAL (strcmp (other.data(), pp), 0);

   adt::DataBlock copy;

   copy = var;
   BOOST_REQUIRE_EQUAL (strcmp (copy.data(), pp), 0);
}

BOOST_AUTO_TEST_CASE( datablock_buffer )
{
   char buffer [128];

   for (int ii = 0; ii < sizeof (buffer); ++ ii)
      buffer [ii] = (char) ii;

   adt::DataBlock var (buffer, sizeof (buffer));

   BOOST_REQUIRE_EQUAL (var.size(), sizeof (buffer));

   for (int ii = 0; ii < sizeof (buffer); ++ ii) {
      BOOST_REQUIRE_EQUAL (buffer [ii], var [ii]);
   }

   BOOST_REQUIRE_EQUAL (var.data(), &buffer[0]);
}

BOOST_AUTO_TEST_CASE( datablock_append )
{
   char buffer [128];
   adt::DataBlock var;

   for (int ii = 0; ii < sizeof (buffer); ++ ii) {
      buffer [ii] = (char) ii;
      var.append((char) ii);
   }

   BOOST_REQUIRE_EQUAL (var.size(), sizeof (buffer));

   for (int ii = 0; ii < sizeof (buffer); ++ ii) {
      BOOST_REQUIRE_EQUAL (buffer [ii], var [ii]);
   }
}

BOOST_AUTO_TEST_CASE( datablock_clear )
{
   adt::DataBlock var ("wow", 3);

   BOOST_REQUIRE_EQUAL (var.size(), 3);

   var.clear();
   BOOST_REQUIRE_EQUAL (var.size(), 0);
   BOOST_REQUIRE_EQUAL (var.empty(), true);
}

BOOST_AUTO_TEST_CASE( datablock_out_of_range )
{
   adt::DataBlock var ("wow", 3);

   char zz;
   BOOST_CHECK_THROW (zz = var [10], adt::RuntimeException);
   BOOST_CHECK_THROW (var [10] = 0, adt::RuntimeException);
}

