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

#include <coffee/basis/pattern/lru/Cache.hpp>
#include <coffee/basis/StreamString.hpp>

#include <boost/test/unit_test.hpp>

using namespace coffee::basis::pattern;

struct LRUCacheFixture {
   static const int MaxCacheSize;

   LRUCacheFixture() : cache(MaxCacheSize) {
      for(int ii = 0; ii < MaxCacheSize; ++ ii) {
         coffee::basis::StreamString string;
         string << "Number-" << ii;
         cache.set(ii, string);
      }
   }

   lru::Cache<int, coffee::basis::StreamString> cache;
};

const int LRUCacheFixture::MaxCacheSize = 5;

BOOST_AUTO_TEST_CASE(lrucache_insert_twice)
{
   const int MaxCacheSize = 5;

   typedef lru::Cache<int, coffee::basis::StreamString> TheCache;

   TheCache cache(MaxCacheSize);

   cache.set(0, "text");
   BOOST_REQUIRE_EQUAL(cache.size(), 1);

   cache.set(0, "other");
   BOOST_REQUIRE_EQUAL(cache.size(), 1);

   BOOST_REQUIRE_EQUAL(cache.value(cache.begin()), "other");
}

BOOST_AUTO_TEST_CASE(lrucache_no_find)
{
   const int MaxCacheSize = 5;

   lru::Cache<int, coffee::basis::StreamString> cache(MaxCacheSize);

   auto ii = cache.find(0);
   BOOST_REQUIRE_EQUAL(ii == cache.end(), true);
}

BOOST_FIXTURE_TEST_CASE(lrucache_find, LRUCacheFixture)
{
   for(int ii = 0; ii < MaxCacheSize; ++ ii) {
      auto ww = cache.find(ii);
      BOOST_REQUIRE_EQUAL(ww == cache.end(), false);
   }
}

BOOST_FIXTURE_TEST_CASE(lrucache_erase, LRUCacheFixture)
{
   BOOST_REQUIRE_EQUAL(cache.erase(3), true);
   BOOST_REQUIRE_EQUAL(cache.size(), MaxCacheSize - 1);

   BOOST_REQUIRE_EQUAL(cache.erase(3), false);
   BOOST_REQUIRE_EQUAL(cache.size(), MaxCacheSize - 1);

   BOOST_REQUIRE_EQUAL(cache.erase(0), true);
   BOOST_REQUIRE_EQUAL(cache.size(), MaxCacheSize - 2);

   std::vector<int> expectedOrder = { 1, 2, 4 };
   int jj = 0;
   for (auto ii = cache.begin(), maxii = cache.end(); ii != maxii; ++ ii) {
      BOOST_REQUIRE_EQUAL(cache.key(ii), expectedOrder[jj ++]);
   }
}

BOOST_FIXTURE_TEST_CASE(lrucache_max_size, LRUCacheFixture)
{
   coffee::basis::StreamString string;
   string << "Number-" << MaxCacheSize;
   cache.set(MaxCacheSize, string);

   BOOST_REQUIRE_EQUAL(cache.find(0) == cache.end(), true);
   BOOST_REQUIRE_EQUAL(cache.size(), MaxCacheSize);

   int jj = 1;
   for (auto ii = cache.begin(), maxii = cache.end(); ii != maxii; ++ ii) {
      coffee::basis::StreamString string;
      string << "Number-" << jj;
      BOOST_REQUIRE_EQUAL(cache.key(ii), jj);
      BOOST_REQUIRE_EQUAL(cache.value(ii), string);
      ++ jj;
   }
}

BOOST_FIXTURE_TEST_CASE(lrucache_update, LRUCacheFixture)
{
   cache.set(0, "new text");

   std::vector<int> expectedOrder = { 1, 2, 3, 4, 0 };
   int jj = 0;
   for (auto ii = cache.begin(), maxii = cache.end(); ii != maxii; ++ ii) {
      BOOST_REQUIRE_EQUAL(cache.key(ii), expectedOrder[jj ++]);
   }
}

BOOST_FIXTURE_TEST_CASE(lrucache_refresh, LRUCacheFixture)
{
   cache.find(0);

   std::vector<int> expectedOrder = { 1, 2, 3, 4, 0 };
   int jj = 0;
   for (auto ii = cache.begin(), maxii = cache.end(); ii != maxii; ++ ii) {
      BOOST_REQUIRE_EQUAL(cache.key(ii), expectedOrder[jj ++]);
   }
}

BOOST_AUTO_TEST_CASE(lrucache_shared_ptr)
{
   typedef std::pair<int,int> Complex;
   typedef std::shared_ptr<Complex> Key;

   const int MaxFirstLevel = 5;
   const int MaxSecondLevel = 5;

   typedef lru::Cache<Key, coffee::basis::StreamString> TheCache;

   TheCache cache(MaxSecondLevel);

   for(int ii = 0; ii < MaxFirstLevel; ++ ii) {
      for (int jj = 0; jj < MaxSecondLevel; ++ jj) {
         Key key = std::make_shared<Complex>(ii, jj);
         coffee::basis::StreamString string;
         string << ii << "-" << jj;
         cache.set(key, string);
      }
   }

   BOOST_REQUIRE_EQUAL(cache.size(), MaxSecondLevel);

   int counter = 0;
   for (TheCache::pair_iterator ii = cache.begin(), maxii = cache.end(); ii != maxii; ++ ii) {
      const Key& key = TheCache::key(ii);
      BOOST_REQUIRE_EQUAL(key->first, MaxFirstLevel - 1);
      BOOST_REQUIRE_EQUAL(key->second, counter ++);
   }
}
