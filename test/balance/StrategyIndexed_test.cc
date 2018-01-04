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
#include <map>
#include <thread>
#include <mutex>

#include <boost/ptr_container/ptr_vector.hpp>

#include <boost/test/unit_test.hpp>

#include <wepa/adt/AsString.hpp>

#include <wepa/logger/Logger.hpp>
#include <wepa/logger/TtyWriter.hpp>

#include <wepa/balance/Resource.hpp>
#include <wepa/balance/StrategyIndexed.hpp>
#include <wepa/balance/GuardResourceList.hpp>
#include <wepa/balance/ResourceList.hpp>

#include <wepa/xml/Node.hpp>
#include <wepa/xml/Compiler.hpp>

#include "TestResource.hpp"

using namespace wepa;
using namespace wepa::balance;
using namespace wepa::test::balance;

namespace IndexedTest {
   typedef std::map <int, int> CounterContainer;
   typedef CounterContainer::iterator counter_iterator;

   static int MaxResources = 10;
   static int MaxLoop = 3;

   void incrementUse (CounterContainer& counterContainer, const std::shared_ptr<Resource>& resource)
   {
      std::shared_ptr<TestResource> myResource = TestResource::cast(resource);

      counter_iterator cc = counterContainer.find (myResource->getKey());

      if (cc == counterContainer.end ()) {
         counterContainer [myResource->getKey()] = 1;
      }
      else {
         cc->second ++;
      }
   }

   void parallel_work(std::mutex& mutexContainer, CounterContainer& counterContainer, balance::StrategyIndexed& strategy)
   {
      for (int ii = 0; ii < MaxResources; ++ ii) {
         auto resource = strategy.apply (ii);

         if (true) {
            std::lock_guard <std::mutex> guard (mutexContainer);
            incrementUse (counterContainer, resource);
         }
      }
   }
}

BOOST_AUTO_TEST_CASE( inx_dont_use_unavailables )
{
   auto resourceList = wepa::test::balance::setup(IndexedTest::MaxResources);

   balance::StrategyIndexed strategy(resourceList);

   std::shared_ptr<TestResource> myResource = TestResource::cast_copy(strategy.apply(0));
   BOOST_REQUIRE_EQUAL (myResource->getKey(), 0);

   if (true) {
      balance::GuardResourceList guard(resourceList);
      std::shared_ptr<TestResource> myResource = TestResource::cast(resourceList->at(guard, 0));
      myResource->setAvailable(false);
   }

   myResource = TestResource::cast_copy(strategy.apply(0));
   BOOST_REQUIRE_EQUAL (myResource->getKey(), 1);

   if (true) {
      balance::GuardResourceList guard(resourceList);
      for (auto ii = resourceList->resource_begin(guard), maxii = resourceList->resource_end(guard); ii != maxii; ++ ii) {
         std::shared_ptr<TestResource> myResource = TestResource::cast(ResourceList::resource(ii));
         myResource->setAvailable(false);
      }
   }

   BOOST_REQUIRE_THROW (strategy.apply(0), ResourceUnavailableException);
}


BOOST_AUTO_TEST_CASE( inx_balance_quality)
{
   auto resourceList = wepa::test::balance::setup(IndexedTest::MaxResources);

   balance::StrategyIndexed strategy(resourceList);
   IndexedTest::CounterContainer counterContainer;

   for (int ii = 0; ii < IndexedTest::MaxResources * IndexedTest::MaxLoop; ++ ii){
      IndexedTest::incrementUse (counterContainer, strategy.apply(ii));
   }

   BOOST_REQUIRE_EQUAL (counterContainer.size (), IndexedTest::MaxResources);

   for (IndexedTest::counter_iterator ii = counterContainer.begin (), maxii = counterContainer.end (); ii != maxii; ++ ii) {
      BOOST_REQUIRE_EQUAL (ii->second, IndexedTest::MaxLoop);
   }
}

BOOST_AUTO_TEST_CASE( inx_balance_multithread )
{
   auto resourceList = wepa::test::balance::setup(IndexedTest::MaxResources);

   balance::StrategyIndexed strategy(resourceList);
   IndexedTest::CounterContainer counterContainer;
   std::mutex mutexContainer;

   std::thread t1(IndexedTest::parallel_work, std::ref (mutexContainer), std::ref (counterContainer), std::ref (strategy));
   std::thread t2(IndexedTest::parallel_work, std::ref (mutexContainer), std::ref (counterContainer), std::ref (strategy));
   std::thread t3(IndexedTest::parallel_work, std::ref (mutexContainer), std::ref (counterContainer), std::ref (strategy));

   t1.join ();
   t2.join ();
   t3.join ();

   int sumUse = 0;

   for (auto& useCounter : counterContainer) {
      BOOST_REQUIRE_GT (useCounter.second, 0);
      sumUse += useCounter.second;
   }

   BOOST_REQUIRE_EQUAL(sumUse, IndexedTest::MaxResources * IndexedTest::MaxLoop);

   BOOST_REQUIRE_EQUAL (counterContainer.size (), IndexedTest::MaxResources);

   for (IndexedTest::counter_iterator ii = counterContainer.begin (), maxii = counterContainer.end (); ii != maxii; ++ ii) {
      BOOST_REQUIRE_EQUAL (ii->second, IndexedTest::MaxLoop);
   }
}
