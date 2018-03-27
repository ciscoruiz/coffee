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

#include <functional>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>

#include <coffee/basis/pattern/lru/Cache.hpp>
#include <coffee/basis/AsString.hpp>
#include <coffee/basis/AsHexString.hpp>

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
      auto field = std::make_shared<dbms::datatype::Integer>(basis::AsHexString::apply(ii));
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
      auto field = std::make_shared<dbms::datatype::Integer>(basis::AsHexString::apply(ii));
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

   typedef basis::pattern::lru::Cache<std::shared_ptr<PrimaryKey>, int, PrimaryKey::HashSharedPointer, PrimaryKey::EqualSharedPointer> Cache;

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
      string->setValue(basis::AsString::apply(ii));

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
