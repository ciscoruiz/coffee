// WEPA - Write Excellent Professional Applications
//
//(c) Copyright 2013 Francisco Ruiz Rayo
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#include <boost/test/unit_test.hpp>

#include <functional>
#include <iostream>
#include <vector>
#include <unordered_map>

#include <wepa/dbms/datatype/Integer.hpp>
#include <wepa/dbms/datatype/String.hpp>

#include <wepa/persistence/PrimaryKey.hpp>
#include <wepa/persistence/PrimaryKeyBuilder.hpp>

using namespace wepa;

BOOST_AUTO_TEST_CASE(persistence_primary_key_compare)
{
   std::shared_ptr<dbms::datatype::Integer> p00;
   std::shared_ptr<dbms::datatype::Integer> p01;
   std::shared_ptr<persistence::PrimaryKey> pk0;

   std::shared_ptr<dbms::datatype::Integer> p10;
   std::shared_ptr<dbms::datatype::Integer> p11;
   std::shared_ptr<persistence::PrimaryKey> pk1;

   std::shared_ptr<dbms::datatype::Integer> p20;
   std::shared_ptr<dbms::datatype::Integer> p21;
   std::shared_ptr<persistence::PrimaryKey> pk2;

   {
      persistence::PrimaryKeyBuilder builder;
      p00 = std::make_shared<dbms::datatype::Integer>("p00");
      p01 = std::make_shared<dbms::datatype::Integer>("p01");
      pk0 = builder.add(p00).add(p01).build();
      p00->setValue(0);
      p01->setValue(2);
   }

   {
      persistence::PrimaryKeyBuilder builder;
      p10 = std::make_shared<dbms::datatype::Integer>("p10");
      p11 = std::make_shared<dbms::datatype::Integer>("p11");
      pk1 = builder.add(p10).add(p11).build();
      p10->setValue(1);
      p11->setValue(1);
   }

   {
      persistence::PrimaryKeyBuilder builder;
      p20 = std::make_shared<dbms::datatype::Integer>("p20");
      p21 = std::make_shared<dbms::datatype::Integer>("p21");
      pk2 = builder.add(p20).add(p21).build();
      p20->setValue(2);
      p21->setValue(0);
   }

   BOOST_REQUIRE_EQUAL(pk0 < pk1, true);
   BOOST_REQUIRE_EQUAL(pk0 < pk2, true);
   BOOST_REQUIRE_EQUAL(pk0 > pk1, false);
   BOOST_REQUIRE_EQUAL(pk0 > pk2, false);

   BOOST_REQUIRE_EQUAL(pk1 < pk0, false);
   BOOST_REQUIRE_EQUAL(pk1 < pk2, false);
   BOOST_REQUIRE_EQUAL(pk1 > pk0, true);
   BOOST_REQUIRE_EQUAL(pk1 > pk2, false);

   BOOST_REQUIRE_EQUAL(pk2 < pk0, false);
   BOOST_REQUIRE_EQUAL(pk2 < pk1, false);
   BOOST_REQUIRE_EQUAL(pk2 > pk0, true);
   BOOST_REQUIRE_EQUAL(pk2 > pk1, true);

   p00->setValue(1);
   p01->setValue(1);
   BOOST_REQUIRE_EQUAL(pk0 == pk1, true);
   BOOST_REQUIRE_EQUAL(pk0 == pk2, false);
}

BOOST_AUTO_TEST_CASE(persistence_primary_key_sort)
{
   std::vector <std::shared_ptr<persistence::PrimaryKey> > pks;

   char name[2];

   for(int key = 0; key < 10; ++ key) {
      persistence::PrimaryKeyBuilder builder;

      auto integer = std::make_shared<dbms::datatype::Integer>("ii");
      integer->setValue(key % 3);

      name[0] = 'a' +(key % 5);
      name[1] = 0;
      auto string = std::make_shared<dbms::datatype::String>(name, 10);
      string->setValue(name);

      pks.push_back(builder.add(integer).add(string).build());
   }

   bool isSorted = true;

   for(int ii = 1; ii < 10; ++ ii) {
      if((pks[ii - 1] < pks[ii]) == false) {
         isSorted = false;
         break;
      }
   }

   BOOST_REQUIRE_EQUAL(isSorted, false);

   try {
      std::sort(pks.begin(), pks.end());
   }
   catch(adt::Exception& ex) {
      std::cout << ex.what() << std::endl;
   }

   isSorted = true;
   for(int ii = 1; ii < 10; ++ ii) {
      if((pks[ii - 1] < pks[ii]) == false) {
         std::cout << "Failed Index=" << ii << std::endl;
         std::cout << "\t" << pks[ii - 1]->asString();
         std::cout << std::endl;
         std::cout << "\t" << pks[ii]->asString();
         std::cout << std::endl;
         isSorted = false;
         break;
      }
   }

   BOOST_REQUIRE_EQUAL(isSorted, true);
}


BOOST_AUTO_TEST_CASE(persistence_primary_key_map)
{
   typedef std::unordered_map<std::shared_ptr<persistence::PrimaryKey>, int> Entries;
   typedef Entries::iterator entries_iterator;
   Entries entries;

   auto integer = std::make_shared<dbms::datatype::Integer>("ii");
   persistence::PrimaryKeyBuilder builder;
   std::shared_ptr<persistence::PrimaryKey> findKey = builder.add(integer).build();

   for(int key = 0; key < 10; ++ key) {
      persistence::PrimaryKeyBuilder builder;

      auto integer = std::make_shared<dbms::datatype::Integer>("ii");
      integer->setValue(key);

      auto pk = builder.add(integer).build();
      entries[pk] = key * key;
   }

   integer->setValue(100);
   BOOST_REQUIRE_EQUAL(entries.find(findKey) == entries.end(), true);

   for(int key = 0; key < 10; ++ key) {
      integer->setValue(key);
      BOOST_REQUIRE_EQUAL(entries.find(findKey) == entries.end(), false);
   }
}
