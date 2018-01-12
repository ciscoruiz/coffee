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
#include <wepa/adt/pattern/lru/Cache.hpp>
#include <wepa/adt/StreamString.hpp>

#include <boost/test/unit_test.hpp>

using namespace wepa::adt::pattern;

BOOST_AUTO_TEST_CASE(lrucache_insert_twice)
{
   const int MaxCacheSize = 5;

   typedef lru::Cache<int, wepa::adt::StreamString> TheCache;

   TheCache cache(MaxCacheSize);

   cache.set(0, "text");
   BOOST_REQUIRE_EQUAL(cache.size(), 1);

   cache.set(0, "other");
   BOOST_REQUIRE_EQUAL(cache.size(), 1);

   BOOST_REQUIRE_EQUAL(cache.value(cache.begin()), "other");
}

BOOST_AUTO_TEST_CASE(lrucache_find)
{
   const int MaxCacheSize = 5;

   typedef lru::Cache<int, wepa::adt::StreamString> TheCache;

   TheCache cache(MaxCacheSize);

   TheCache::pair_iterator ii = cache.find(0);
   BOOST_REQUIRE_EQUAL(ii == cache.end(), true);

   for(int ii = 0; ii < MaxCacheSize; ++ ii) {
      wepa::adt::StreamString string;
      string << "Number-" << ii;
      cache.set(ii, string);
   }

   for(int ii = 0; ii < MaxCacheSize; ++ ii) {
      TheCache::pair_iterator ww = cache.find(ii);
      BOOST_REQUIRE_EQUAL(ww == cache.end(), false);
   }
}

BOOST_AUTO_TEST_CASE(lrucache_erase)
{
   const int MaxCacheSize = 5;

   typedef lru::Cache<int, wepa::adt::StreamString> TheCache;

   TheCache cache(MaxCacheSize);

   for(int ii = 0; ii < MaxCacheSize; ++ ii) {
      wepa::adt::StreamString string;
      string << "Number-" << ii;
      cache.set(ii, string);
   }

   BOOST_REQUIRE_EQUAL(cache.erase(3), true);
   BOOST_REQUIRE_EQUAL(cache.size(), MaxCacheSize - 1);

   BOOST_REQUIRE_EQUAL(cache.erase(3), false);
   BOOST_REQUIRE_EQUAL(cache.size(), MaxCacheSize - 1);

   BOOST_REQUIRE_EQUAL(cache.erase(0), true);
   BOOST_REQUIRE_EQUAL(cache.size(), MaxCacheSize - 2);

   std::vector<int> expectedOrder = { 1, 2, 4 };
   int jj = 0;
   for (TheCache::pair_iterator ii = cache.begin(), maxii = cache.end(); ii != maxii; ++ ii) {
      BOOST_REQUIRE_EQUAL(TheCache::key(ii), expectedOrder[jj ++]);
   }
}

BOOST_AUTO_TEST_CASE(lrucache_max_size)
{
   const int MaxCacheSize = 5;

   typedef lru::Cache<int, wepa::adt::StreamString> TheCache;

   TheCache cache(MaxCacheSize);

   for(int ii = 0; ii < MaxCacheSize + 1; ++ ii) {
      wepa::adt::StreamString string;
      string << "Number-" << ii;
      cache.set(ii, string);
   }

   BOOST_REQUIRE_EQUAL(cache.find(0) == cache.end(), true);
   BOOST_REQUIRE_EQUAL(cache.size(), MaxCacheSize);

   int jj = 1;
   for (TheCache::pair_iterator ii = cache.begin(), maxii = cache.end(); ii != maxii; ++ ii) {
      wepa::adt::StreamString string;
      string << "Number-" << jj;
      BOOST_REQUIRE_EQUAL(TheCache::key(ii), jj);
      BOOST_REQUIRE_EQUAL(TheCache::value(ii), string);
      ++ jj;
   }
}

BOOST_AUTO_TEST_CASE(lrucache_update)
{
   const int MaxCacheSize = 5;

   typedef lru::Cache<int, wepa::adt::StreamString> TheCache;

   TheCache cache(MaxCacheSize);

   for(int ii = 0; ii < MaxCacheSize; ++ ii) {
      wepa::adt::StreamString string;
      string << "Number-" << ii;
      cache.set(ii, string);
   }

   cache.set(0, "new text");

   std::vector<int> expectedOrder = { 1, 2, 3, 4, 0 };
   int jj = 0;
   for (TheCache::pair_iterator ii = cache.begin(), maxii = cache.end(); ii != maxii; ++ ii) {
      BOOST_REQUIRE_EQUAL(TheCache::key(ii), expectedOrder[jj ++]);
   }
}

BOOST_AUTO_TEST_CASE(lrucache_refresh)
{
   const int MaxCacheSize = 5;

   typedef lru::Cache<int, wepa::adt::StreamString> TheCache;

   TheCache cache(MaxCacheSize);

   for(int ii = 0; ii < MaxCacheSize; ++ ii) {
      wepa::adt::StreamString string;
      string << "Number-" << ii;
      cache.set(ii, string);
   }

   cache.find(0);

   std::vector<int> expectedOrder = { 1, 2, 3, 4, 0 };
   int jj = 0;
   for (TheCache::pair_iterator ii = cache.begin(), maxii = cache.end(); ii != maxii; ++ ii) {
      BOOST_REQUIRE_EQUAL(TheCache::key(ii), expectedOrder[jj ++]);
   }
}


BOOST_AUTO_TEST_CASE(lrucache_shared_ptr)
{
   typedef std::pair<int,int> Complex;
   typedef std::shared_ptr<Complex> Key;

   const int MaxFirstLevel = 5;
   const int MaxSecondLevel = 5;

   typedef lru::Cache<Key, wepa::adt::StreamString> TheCache;

   TheCache cache(MaxSecondLevel);

   for(int ii = 0; ii < MaxFirstLevel; ++ ii) {
      for (int jj = 0; jj < MaxSecondLevel; ++ jj) {
         Key key = std::make_shared<Complex>(ii, jj);
         wepa::adt::StreamString string;
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
