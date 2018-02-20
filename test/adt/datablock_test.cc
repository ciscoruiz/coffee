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

#include <coffee/adt/DataBlock.hpp>
#include <coffee/adt/RuntimeException.hpp>

using namespace std;
using namespace coffee;

BOOST_AUTO_TEST_CASE( datablock_size )
{
   adt::DataBlock var ("hello xxx", 5);

   BOOST_REQUIRE_EQUAL (var.size (), 5);

   var.assign("second");

   BOOST_REQUIRE_EQUAL (var.size (), 6);

   var.assign ("three", 2);
   BOOST_REQUIRE_EQUAL (var.size (), 2);
}

BOOST_AUTO_TEST_CASE( datablock_copy )
{
   const char* pp = "hello xxx";

   adt::DataBlock var (pp, strlen (pp) + 1);

   BOOST_REQUIRE_EQUAL (var.size (), strlen (pp) + 1);

   adt::DataBlock other (var);

   BOOST_REQUIRE_EQUAL (strcmp (other.data(), pp), 0);

   adt::DataBlock copy;
   copy = var;
   BOOST_REQUIRE(copy == var);
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

BOOST_AUTO_TEST_CASE( datablock_operator_parenthesis )
{
   adt::DataBlock var ("wow", 3);

   var [1] = 'i';

   const adt::DataBlock copy (var);

   BOOST_REQUIRE_EQUAL (strcmp (var.data (), "wiw"), 0);
   BOOST_REQUIRE_EQUAL (copy [1], 'i');
   BOOST_REQUIRE_EQUAL (var [1], 'i');
}

BOOST_AUTO_TEST_CASE( datablock_out_of_range )
{
   adt::DataBlock var ("wow", 3);

   char zz;
   BOOST_CHECK_THROW (zz = var [10], adt::RuntimeException);
   BOOST_CHECK_THROW (var [10] = 0, adt::RuntimeException);

   const adt::DataBlock copy (var);
   BOOST_CHECK_THROW (copy [10] == 0, adt::RuntimeException);
   BOOST_CHECK_THROW (copy.at(10), adt::RuntimeException);

}
