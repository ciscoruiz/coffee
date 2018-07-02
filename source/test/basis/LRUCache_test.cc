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

#include <memory>

#include <coffee/basis/pattern/lru/Cache.hpp>
#include <coffee/basis/StreamString.hpp>

#include <gtest/gtest.h>

using namespace coffee::basis::pattern;

struct LRUCacheFixture : public ::testing::Test {
   static const int MaxCacheSize;

   LRUCacheFixture() : cache(MaxCacheSize) {
   }

   void SetUp() {
      for(int ii = 0; ii < MaxCacheSize; ++ ii) {
         coffee::basis::StreamString string;
         string << "Number-" << ii;
         cache.set(ii, string);
      }
   }

   lru::Cache<int, coffee::basis::StreamString> cache;
};

const int LRUCacheFixture::MaxCacheSize = 5;

TEST(LruCacheTest, insert_twice)
{
   const int MaxCacheSize = 5;

   typedef lru::Cache<int, coffee::basis::StreamString> TheCache;

   TheCache cache(MaxCacheSize);

   cache.set(0, "text");
   ASSERT_EQ(1, cache.size());

   cache.set(0, "other");
   ASSERT_EQ(1, cache.size());

   ASSERT_EQ("other", cache.value(cache.begin()));
}

TEST(LruCacheTest, no_find)
{
   const int MaxCacheSize = 5;

   lru::Cache<int, coffee::basis::StreamString> cache(MaxCacheSize);

   auto ii = cache.find(0);
   ASSERT_TRUE(ii == cache.end());
}

TEST_F(LRUCacheFixture, lrucache_find)
{
   for(int ii = 0; ii < MaxCacheSize; ++ ii) {
      auto ww = cache.find(ii);
      ASSERT_FALSE(ww == cache.end());
   }
}

TEST_F(LRUCacheFixture, lrucache_erase)
{
   ASSERT_TRUE(cache.erase(3));
   ASSERT_EQ(MaxCacheSize - 1, cache.size());

   ASSERT_FALSE(cache.erase(3));
   ASSERT_EQ(MaxCacheSize - 1, cache.size());

   ASSERT_TRUE(cache.erase(0));
   ASSERT_EQ(MaxCacheSize - 2, cache.size());

   std::vector<int> expectedOrder = { 1, 2, 4 };
   int jj = 0;
   for (auto ii = cache.begin(), maxii = cache.end(); ii != maxii; ++ ii) {
      ASSERT_EQ(expectedOrder[jj ++], cache.key(ii));
   }
}

TEST_F(LRUCacheFixture, lrucache_max_size)
{
   coffee::basis::StreamString string;
   string << "Number-" << MaxCacheSize;
   cache.set(MaxCacheSize, string);

   ASSERT_TRUE(cache.find(0) == cache.end());
   ASSERT_EQ(MaxCacheSize, cache.size());

   int jj = 1;
   for (auto ii = cache.begin(), maxii = cache.end(); ii != maxii; ++ ii) {
      coffee::basis::StreamString string;
      string << "Number-" << jj;
      ASSERT_EQ(jj, cache.key(ii));
      ASSERT_EQ(string, cache.value(ii));
      ++ jj;
   }
}

TEST_F(LRUCacheFixture, lrucache_update)
{
   cache.set(0, "new text");

   std::vector<int> expectedOrder = { 1, 2, 3, 4, 0 };
   int jj = 0;
   for (auto ii = cache.begin(), maxii = cache.end(); ii != maxii; ++ ii) {
      ASSERT_EQ(expectedOrder[jj ++], cache.key(ii));
   }
}

TEST_F(LRUCacheFixture, lrucache_refresh)
{
   cache.find(0);

   std::vector<int> expectedOrder = { 1, 2, 3, 4, 0 };
   int jj = 0;
   for (auto ii = cache.begin(), maxii = cache.end(); ii != maxii; ++ ii) {
      ASSERT_EQ(expectedOrder[jj ++], cache.key(ii));
   }
}

TEST(LruCacheTest, shared_ptr)
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

   ASSERT_EQ(cache.size(), MaxSecondLevel);

   int counter = 0;
   for (TheCache::pair_iterator ii = cache.begin(), maxii = cache.end(); ii != maxii; ++ ii) {
      const Key& key = TheCache::key(ii);
      ASSERT_EQ(MaxFirstLevel - 1, key->first);
      ASSERT_EQ(counter ++, key->second);
   }
}
