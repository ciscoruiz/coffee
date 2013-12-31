// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2013 Francisco Ruiz Rayo
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
#include <boost/test/unit_test.hpp>

#include <wepa/adt/DataBlock.hpp>
#include <wepa/adt/RuntimeException.hpp>

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
   BOOST_CHECK_THROW (var [10] == 0, adt::RuntimeException);
}
