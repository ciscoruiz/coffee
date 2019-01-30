// MIT License
// 
// Copyright(c) 2018 Francisco Ruiz(francisco.ruiz.rayo@gmail.com)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
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

#include <map>
#include <thread>
#include <mutex>

#include <gtest/gtest.h>

#include <coffee/basis/AsString.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TtyWriter.hpp>

#include <coffee/balance/Resource.hpp>
#include <coffee/balance/StrategyRoundRobin.hpp>
#include <coffee/balance/GuardResourceContainer.hpp>
#include <coffee/xml/Compiler.hpp>
#include <coffee/xml/Node.hpp>

#include "TestResource.hpp"
#include "ResourceListFixture.hpp"

using namespace coffee;
using namespace coffee::balance;
using namespace coffee::test::balance;

namespace RoundRobinTest {
   typedef std::map <int, int> CounterContainer;
   typedef CounterContainer::iterator counter_iterator;

   static int MaxLoop = 5;

   void incrementUse(CounterContainer& counterContainer, const std::shared_ptr<Resource>& resource)
   {
      std::shared_ptr<TestResource> myResource = TestResource::cast(resource);

      counter_iterator cc = counterContainer.find(myResource->getKey());

      if(cc == counterContainer.end()) {
         counterContainer [myResource->getKey()] = 1;
      }
      else {
         cc->second ++;
      }
   }

    class Identifier : public balance::Strategy::Request {
    public:
       Identifier() {;}
        int calculateIdentifier() const noexcept { return  0;}
    };

    void parallel_work(std::mutex& mutexContainer, CounterContainer& counterContainer, balance::StrategyRoundRobin& strategy)
   {
      for(int ii = 0; ii < ResourceListFixture::MaxResources; ++ ii) {
         auto resource = strategy.apply(Identifier());

         if(true) {
            std::lock_guard <std::mutex> guard(mutexContainer);
            incrementUse(counterContainer, resource);
         }
      }
   }
}

struct StrategyRoundRobinFixture : public ResourceListFixture {};

TEST_F(StrategyRoundRobinFixture, dont_use_unavailables)
{
   balance::StrategyRoundRobin strategy(resourceList);

   if(true) {
      balance::GuardResourceContainer guard(resourceList);
      std::shared_ptr<TestResource> myResource = TestResource::cast(resourceList->at(guard, 0));
      myResource->setAvailable(false);
   }

   std::shared_ptr<TestResource> myResource = TestResource::cast_copy(strategy.apply(RoundRobinTest::Identifier()));
   ASSERT_EQ(1, myResource->getKey());

   if(true) {
      balance::GuardResourceContainer guard(resourceList);
      for(auto ii = resourceList->resource_begin(guard), maxii = resourceList->resource_end(guard); ii != maxii; ++ ii) {
         std::shared_ptr<TestResource> myResource = TestResource::cast(ResourceContainer::resource(ii));
         myResource->setAvailable(false);
      }
   }


   ASSERT_THROW(strategy.apply(RoundRobinTest::Identifier()), ResourceUnavailableException);
}

TEST_F(StrategyRoundRobinFixture, balance_quality)
{
   balance::StrategyRoundRobin strategy(resourceList);
   RoundRobinTest::CounterContainer counterContainer;

   for(int ii = 0; ii < MaxResources * RoundRobinTest::MaxLoop; ++ ii){
      RoundRobinTest::incrementUse(counterContainer, strategy.apply(RoundRobinTest::Identifier()));
   }

   ASSERT_EQ(MaxResources, counterContainer.size());

   for(RoundRobinTest::counter_iterator ii = counterContainer.begin(), maxii = counterContainer.end(); ii != maxii; ++ ii) {
      ASSERT_EQ(RoundRobinTest::MaxLoop, ii->second);
   }
}

TEST(StrategyRoundRobinTest, empty_strategy)
{
   auto emptyList = coffee::balance::ResourceContainer::instantiate("EmptyList");
   balance::StrategyRoundRobin strategy(emptyList);
   ASSERT_THROW(strategy.apply(RoundRobinTest::Identifier()), ResourceUnavailableException);
}

TEST_F(StrategyRoundRobinFixture, asXML)
{
   balance::StrategyRoundRobin strategy(resourceList);
   xml::Compiler compiler;
   auto root = std::make_shared<xml::Node>("root");
   ASSERT_EQ(682, compiler.apply(strategy.asXML(root)).size());
}

TEST_F(StrategyRoundRobinFixture, balance_multithread)
{
   balance::StrategyRoundRobin strategy(resourceList);
   RoundRobinTest::CounterContainer counterContainer;
   std::mutex mutexContainer;
   std::vector<std::shared_ptr<std::thread> > threads;

   for(int ii = 0; ii < RoundRobinTest::MaxLoop; ++ ii) {
      threads.push_back(std::make_shared<std::thread>(RoundRobinTest::parallel_work, std::ref(mutexContainer), std::ref(counterContainer), std::ref(strategy)));
   }

   for(auto ii = threads.begin(), maxii = threads.end(); ii != maxii; ++ ii) {
     (*ii)->join();
   }

   int sumUse = 0;
   for(auto& useCounter : counterContainer) {
      ASSERT_GT(useCounter.second, 0);
      sumUse += useCounter.second;
   }

   ASSERT_EQ(MaxResources * RoundRobinTest::MaxLoop, sumUse);

   ASSERT_EQ(MaxResources, counterContainer.size());

   for(RoundRobinTest::counter_iterator ii = counterContainer.begin(), maxii = counterContainer.end(); ii != maxii; ++ ii) {
      ASSERT_EQ(RoundRobinTest::MaxLoop, ii->second);
   }
}

