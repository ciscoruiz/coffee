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
#include <iostream>
#include <time.h>

#include <boost/test/unit_test.hpp>

#include <wepa/dbms/datatype/Integer.hpp>
#include <wepa/dbms/datatype/Float.hpp>

using namespace wepa;
using namespace wepa::dbms;


BOOST_AUTO_TEST_CASE (integer_is_nulleable)
{
   datatype::Integer column ("nulleable", true);

   BOOST_REQUIRE_EQUAL (column.hasValue (), false);

   column.clear ();

   BOOST_REQUIRE_EQUAL (column.hasValue (), false);

   BOOST_REQUIRE_THROW (column.getValue (), adt::RuntimeException);
   BOOST_REQUIRE_THROW (column + 2, adt::RuntimeException);

   column.setValue (10);
   BOOST_REQUIRE_EQUAL (column.hasValue (), true);
   BOOST_REQUIRE_EQUAL (column.getValue(),10);

   column.clear ();
   BOOST_REQUIRE_EQUAL (column.hasValue (), false);
}

BOOST_AUTO_TEST_CASE (integer_is_not_nulleable)
{
   datatype::Integer column ("not_nulleable", false);

   BOOST_REQUIRE_EQUAL (column.hasValue (), true);

   column.setValue (11);
   BOOST_REQUIRE_EQUAL (column.hasValue (), true);
   BOOST_REQUIRE_EQUAL (column.getValue(), 11);
   BOOST_REQUIRE_EQUAL (column + 10, 21);

   column.clear();
   BOOST_REQUIRE_EQUAL (column.hasValue(), true);
}

BOOST_AUTO_TEST_CASE (integer_downcast)
{
   datatype::Integer column ("not_nulleable");

   datatype::Abstract& abs = column;

   datatype::Integer& other = wepa_datatype_downcast(datatype::Integer, abs);

   BOOST_REQUIRE_EQUAL (&other, &column);

   datatype::Float zzz ("zzz");

   BOOST_REQUIRE_THROW(wepa_datatype_downcast(datatype::Integer, zzz), adt::RuntimeException);
}