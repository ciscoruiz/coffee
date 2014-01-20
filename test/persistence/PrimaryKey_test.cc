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

#include <functional>
#include <iostream>

#include <boost/ptr_container/ptr_vector.hpp>

#include <wepa/dbms/datatype/Integer.hpp>
#include <wepa/dbms/datatype/String.hpp>

#include <wepa/persistence/PrimaryKey.hpp>

using namespace wepa;

BOOST_AUTO_TEST_CASE (persistence_primary_key_compare)
{
   boost::ptr_vector <dbms::datatype::Abstract> values;
   dbms::datatype::Integer* ivalue;

   persistence::PrimaryKey pk0;
   persistence::PrimaryKey pk1;
   persistence::PrimaryKey pk2;

   BOOST_REQUIRE_EQUAL (pk0.isDefined (), false);

   BOOST_REQUIRE_THROW (pk0 == pk1, adt::RuntimeException);

   values.push_back (ivalue = new dbms::datatype::Integer ("p0"));
   pk0.addComponent(ivalue);
   ivalue->setValue (0);

   values.push_back (ivalue = new dbms::datatype::Integer ("p0-i0"));
   pk0.addComponent(ivalue);
   ivalue->setValue (0);

   BOOST_REQUIRE_EQUAL (pk0.isDefined (), true);

   values.push_back (ivalue = new dbms::datatype::Integer ("p0"));
   pk1.addComponent(ivalue);
   ivalue->setValue (0);

   values.push_back (ivalue = new dbms::datatype::Integer ("p0-i1"));
   pk1.addComponent(ivalue);
   ivalue->setValue (1);

   values.push_back (ivalue = new dbms::datatype::Integer ("p0"));
   pk2.addComponent(ivalue);
   ivalue->setValue (0);

   values.push_back (ivalue = new dbms::datatype::Integer ("p0-i2"));
   pk2.addComponent(ivalue);
   ivalue->setValue (2);

   BOOST_REQUIRE_EQUAL (pk0 < pk1, true);
   BOOST_REQUIRE_EQUAL (pk0 < pk2, true);

   BOOST_REQUIRE_EQUAL (pk1 < pk0, false);
   BOOST_REQUIRE_EQUAL (pk2 < pk0, false);

   BOOST_REQUIRE_EQUAL (pk1 < pk0, false);
   BOOST_REQUIRE_EQUAL (pk1 < pk2, true);

   BOOST_REQUIRE_EQUAL (pk0 < pk1, true);
   BOOST_REQUIRE_EQUAL (pk2 < pk1, false);

   BOOST_REQUIRE_EQUAL (pk2 < pk0, false);
   BOOST_REQUIRE_EQUAL (pk2 < pk1, false);

   BOOST_REQUIRE_EQUAL (pk0 < pk2, true);
   BOOST_REQUIRE_EQUAL (pk1 < pk2, true);
}

struct PrimaryKeyTestPtrLess {
   bool operator () (const persistence::PrimaryKey& first, const persistence::PrimaryKey& second) throw (adt::RuntimeException) {
      return first < second;
   }
};

BOOST_AUTO_TEST_CASE (persistence_primary_key_sort)
{
   boost::ptr_vector <dbms::datatype::Abstract> values;
   boost::ptr_vector <persistence::PrimaryKey> keys;

   char name [2];

   for (int key = 0; key < 10; ++ key) {
      persistence::PrimaryKey* primaryKey = new persistence::PrimaryKey;

      keys.push_back (primaryKey);

      dbms::datatype::Integer* ivalue = new dbms::datatype::Integer ("ii");
      ivalue->setValue (key % 3);
      values.push_back (ivalue);

      name [0] = 'a' + (key % 5);
      name [1] = 0;
      dbms::datatype::String* svalue = new dbms::datatype::String (name, 10);
      svalue->setValue(name);
      values.push_back (svalue);

      primaryKey->addComponent(ivalue);
      primaryKey->addComponent(svalue);
   }

   bool isSorted = true;

   for (int ii = 1; ii < 10; ++ ii) {
      if ((keys [ii - 1] < keys [ii]) == false) {
         std::cout << "Unsorted Index=" << ii << std::endl;
         std::cout << "\t" << keys [ii - 1].asString ();
         std::cout << std::endl;
         std::cout << "\t" << keys [ii].asString ();
         std::cout << std::endl;
         isSorted = false;
         break;
      }
   }

   BOOST_REQUIRE_EQUAL (isSorted, false);

   std::sort (keys.begin (), keys.end (), PrimaryKeyTestPtrLess ());

   isSorted = true;
   for (int ii = 1; ii < 10; ++ ii) {
      if ((keys [ii - 1] < keys [ii]) == false) {
         std::cout << "Failed Index=" << ii << std::endl;
         std::cout << "\t" << keys [ii - 1].asString ();
         std::cout << std::endl;
         std::cout << "\t" << keys [ii].asString ();
         std::cout << std::endl;
         isSorted = false;
         break;
      }
   }

   BOOST_REQUIRE_EQUAL (isSorted, true);
}

