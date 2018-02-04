// COFFEE - COmpany eFFEEctive Platform
//
//(c) Copyright 2018 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/coffee
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
#include <map>

#include <coffee/adt/pattern/lru/Cache.hpp>
#include <coffee/adt/AsString.hpp>
#include <coffee/adt/AsHexString.hpp>

#include <coffee/dbms/datatype/Integer.hpp>
#include <coffee/dbms/datatype/String.hpp>

#include <coffee/persistence/PrimaryKey.hpp>
#include <coffee/persistence/PrimaryKeyBuilder.hpp>

using namespace coffee;

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
      p00 = std::make_shared<dbms::datatype::Integer>("first");
      p01 = std::make_shared<dbms::datatype::Integer>("second");
      pk0 = builder.add(p00).add(p01).build();
      p00->setValue(0);
      p01->setValue(2);
   }

   {
      persistence::PrimaryKeyBuilder builder;
      p10 = std::make_shared<dbms::datatype::Integer>("first");
      p11 = std::make_shared<dbms::datatype::Integer>("second");
      pk1 = builder.add(p10).add(p11).build();
      p10->setValue(1);
      p11->setValue(1);
   }

   {
      persistence::PrimaryKeyBuilder builder;
      p20 = std::make_shared<dbms::datatype::Integer>("first");
      p21 = std::make_shared<dbms::datatype::Integer>("second");
      pk2 = builder.add(p20).add(p21).build();
      p20->setValue(2);
      p21->setValue(0);
   }

   BOOST_REQUIRE_EQUAL(pk0->compare(pk1) < 0, true);
   BOOST_REQUIRE_EQUAL(pk0->compare(pk2) < 0, true);
   BOOST_REQUIRE_EQUAL(pk0->compare(pk1) > 0, false);
   BOOST_REQUIRE_EQUAL(pk0->compare(pk2) > 0, false);

   BOOST_REQUIRE_EQUAL(pk1->compare(pk0) < 0, false);
   BOOST_REQUIRE_EQUAL(pk1->compare(pk2) < 0, true);
   BOOST_REQUIRE_EQUAL(pk1->compare(pk0) > 0, true);
   BOOST_REQUIRE_EQUAL(pk1->compare(pk2) > 0, false);

   BOOST_REQUIRE_EQUAL(pk2->compare(pk0) < 0, false);
   BOOST_REQUIRE_EQUAL(pk2->compare(pk1) < 0, false);
   BOOST_REQUIRE_EQUAL(pk2->compare(pk0) > 0, true);
   BOOST_REQUIRE_EQUAL(pk2->compare(pk1) > 0, true);

   p00->setValue(1);
   p01->setValue(1);

   BOOST_REQUIRE_EQUAL(pk0->compare(pk1), 0);
   BOOST_REQUIRE_NE(pk0->compare(pk2), 0);
}


// See https://stackoverflow.com/questions/17572583/boost-check-fails-to-compile-operator-for-custom-types
namespace boost {
namespace test_tools {
   template<> struct print_log_value<persistence::PrimaryKey> {
      void operator()( std::ostream& os, persistence::PrimaryKey const& ts)
      {
          os << ts.asString();
      }
   };
}}

BOOST_AUTO_TEST_CASE(persistence_primary_key_copy_constructor)
{
   persistence::PrimaryKeyBuilder builder;

   for (int ii = 0; ii < 25; ++ ii) {
      auto field = std::make_shared<dbms::datatype::Integer>(adt::AsHexString::apply(ii));
      builder.add(field);
   }

   persistence::PrimaryKey firstKey(builder);
   persistence::PrimaryKey secondKey(firstKey);

   BOOST_REQUIRE_EQUAL(firstKey, secondKey);
}

BOOST_AUTO_TEST_CASE(persistence_primary_key_assignmet_operator)
{
   persistence::PrimaryKeyBuilder builder;

   for (int ii = 0; ii < 25; ++ ii) {
      auto field = std::make_shared<dbms::datatype::Integer>(adt::AsHexString::apply(ii));
      builder.add(field);
   }

   persistence::PrimaryKey firstKey(builder);

   persistence::PrimaryKeyBuilder otherBuilder;
   builder.add(std::make_shared<dbms::datatype::String>("string", 16));
   persistence::PrimaryKey secondKey(otherBuilder);

   secondKey = firstKey;

   BOOST_REQUIRE_EQUAL(firstKey, secondKey);
}

BOOST_AUTO_TEST_CASE(persistence_primary_key_map)
{
   using persistence::PrimaryKey;

   typedef adt::pattern::lru::Cache<std::shared_ptr<PrimaryKey>, int, PrimaryKey::HashSharedPointer, PrimaryKey::EqualSharedPointer> Cache;

   Cache entries(16);

   auto integer = std::make_shared<dbms::datatype::Integer>("ii");
   persistence::PrimaryKeyBuilder builder;
   std::shared_ptr<persistence::PrimaryKey> findKey = builder.add(integer).build();

   for(int key = 0; key < 10; ++ key) {
      persistence::PrimaryKeyBuilder builder;
      auto integer = std::make_shared<dbms::datatype::Integer>("ii");
      integer->setValue(key);
      auto pk = builder.add(integer).build();
      entries.set(pk, key * key);
   }

   BOOST_REQUIRE_EQUAL(entries.size(), 10);

   for(int key = 100; key < 110; ++ key) {
      integer->setValue(key);
      BOOST_REQUIRE_EQUAL(entries.find(findKey) == entries.end(), true);
   }

   BOOST_REQUIRE_EQUAL(entries.size(), 10);

   for(int key = 0; key < 10; ++ key) {
      integer->setValue(key);
      BOOST_REQUIRE_EQUAL(entries.find(findKey) == entries.end(), false);
   }
}

BOOST_AUTO_TEST_CASE(persistence_primary_key_hash)
{
   static const int maxSize = 10000;

   std::map<size_t, int> collisions;

   for (int ii = 0; ii < maxSize; ++ ii) {
      auto integer = std::make_shared<dbms::datatype::Integer>("integer");
      auto string = std::make_shared<dbms::datatype::String>("string", 16);

      integer->setValue(ii);
      string->setValue(adt::AsString::apply(ii));

      persistence::PrimaryKeyBuilder builder;
      std::shared_ptr<persistence::PrimaryKey> pk = builder.add(string).add(integer).build();

      auto hash = pk->hash();

      if (collisions.find(hash) == collisions.end()) {
         collisions[hash] = 1;
      }
      else {
         collisions[hash] ++;
      }
   }

   BOOST_CHECK_GT(collisions.size(), maxSize * 90 / 100);

}
