// COFFEE - COmpany eFFEEctive Platform
//
// (c) Copyright 2018 Francisco Ruiz Rayo
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

#include <coffee/adt/AsString.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TtyWriter.hpp>

#include <coffee/balance/Resource.hpp>
#include <coffee/balance/StrategyRoundRobin.hpp>
#include <coffee/balance/GuardResourceList.hpp>

#include "TestResource.hpp"

using namespace coffee;
using namespace coffee::balance;
using namespace coffee::test::balance;

namespace RoundRobinTest {
   typedef std::map <int, int> CounterContainer;
   typedef CounterContainer::iterator counter_iterator;

   static int MaxResources = 13;
   static int MaxLoop = 5;

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

   void parallel_work(std::mutex& mutexContainer, CounterContainer& counterContainer, balance::StrategyRoundRobin& strategy)
   {
      for (int ii = 0; ii < MaxResources; ++ ii) {
         auto resource = strategy.apply ();

         if (true) {
            std::lock_guard <std::mutex> guard (mutexContainer);
            incrementUse (counterContainer, resource);
         }
      }
   }
}

BOOST_AUTO_TEST_CASE( rr_dont_use_unavailables )
{
   auto resourceList = coffee::test::balance::setup(RoundRobinTest::MaxResources);

   balance::StrategyRoundRobin strategy(resourceList);

   if (true) {
      balance::GuardResourceList guard(resourceList);
      std::shared_ptr<TestResource> myResource = TestResource::cast(resourceList->at(guard, 0));
      myResource->setAvailable(false);
   }

   std::shared_ptr<TestResource> myResource = TestResource::cast_copy(strategy.apply());
   BOOST_REQUIRE_EQUAL (myResource->getKey(), 1);

   if (true) {
      balance::GuardResourceList guard(resourceList);
      for (auto ii = resourceList->resource_begin(guard), maxii = resourceList->resource_end(guard); ii != maxii; ++ ii) {
         std::shared_ptr<TestResource> myResource = TestResource::cast(ResourceList::resource(ii));
         myResource->setAvailable(false);
      }
   }

   BOOST_REQUIRE_THROW (strategy.apply(), ResourceUnavailableException);
}

BOOST_AUTO_TEST_CASE( rr_balance_quality)
{
   auto resourceList = coffee::test::balance::setup(RoundRobinTest::MaxResources);

   balance::StrategyRoundRobin strategy(resourceList);
   RoundRobinTest::CounterContainer counterContainer;

   for (int ii = 0; ii < RoundRobinTest::MaxResources * RoundRobinTest::MaxLoop; ++ ii){
      RoundRobinTest::incrementUse (counterContainer, strategy.apply());
   }

   BOOST_REQUIRE_EQUAL (counterContainer.size (), RoundRobinTest::MaxResources);

   for (RoundRobinTest::counter_iterator ii = counterContainer.begin (), maxii = counterContainer.end (); ii != maxii; ++ ii) {
      BOOST_REQUIRE_EQUAL (ii->second, RoundRobinTest::MaxLoop);
   }
}

BOOST_AUTO_TEST_CASE( rr_balance_multithread )
{
   auto resourceList = coffee::test::balance::setup(RoundRobinTest::MaxResources);

   balance::StrategyRoundRobin strategy(resourceList);
   RoundRobinTest::CounterContainer counterContainer;
   std::mutex mutexContainer;
   std::vector<std::shared_ptr<std::thread> > threads;

   for (int ii = 0; ii < RoundRobinTest::MaxLoop; ++ ii) {
      threads.push_back(std::make_shared<std::thread>(RoundRobinTest::parallel_work, std::ref (mutexContainer), std::ref (counterContainer), std::ref (strategy)));
   }

   for(auto ii = threads.begin(), maxii = threads.end(); ii != maxii; ++ ii) {
      (*ii)->join();
   }

   int sumUse = 0;
   for (auto& useCounter : counterContainer) {
      BOOST_REQUIRE_GT (useCounter.second, 0);
      sumUse += useCounter.second;
   }

   BOOST_REQUIRE_EQUAL(sumUse, RoundRobinTest::MaxResources * RoundRobinTest::MaxLoop);

   BOOST_REQUIRE_EQUAL (counterContainer.size (), RoundRobinTest::MaxResources);

   for (RoundRobinTest::counter_iterator ii = counterContainer.begin (), maxii = counterContainer.end (); ii != maxii; ++ ii) {
      BOOST_REQUIRE_EQUAL (ii->second, RoundRobinTest::MaxLoop);
   }
}
