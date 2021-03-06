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

#include <boost/ptr_container/ptr_vector.hpp>

#include <gtest/gtest.h>

#include <coffee/basis/AsString.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TtyWriter.hpp>

#include <coffee/balance/Resource.hpp>
#include <coffee/balance/StrategyIndexed.hpp>
#include <coffee/balance/GuardResourceList.hpp>
#include <coffee/balance/ResourceList.hpp>

#include <coffee/xml/Node.hpp>
#include <coffee/xml/Compiler.hpp>

#include "TestResource.hpp"
#include "ResourceListFixture.hpp"

using namespace coffee;
using namespace coffee::balance;
using namespace coffee::test::balance;

namespace IndexedTest {
   typedef std::map <int, int> CounterContainer;
   typedef CounterContainer::iterator counter_iterator;

   static int MaxResources = 10;
   static int MaxLoop = 3;

  class Identifier : public balance::Strategy::Request {
  public:
      explicit Identifier(const int value) : m_value(value){;}

      int calculateIdentifier() const noexcept { return m_value;   }

  private:
      const int m_value;
  };

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

   void parallel_work(std::mutex& mutexContainer, CounterContainer& counterContainer, balance::StrategyIndexed& strategy)
   {
      for(int ii = 0; ii < MaxResources; ++ ii) {
          Identifier id(ii);
         auto resource = strategy.apply(id);

         if(true) {
            std::lock_guard <std::mutex> guard(mutexContainer);
            incrementUse(counterContainer, resource);
         }
      }
   }
}

struct StrategyIndexedFixture : public ResourceListFixture {};

TEST_F(StrategyIndexedFixture, dont_use_unavailables)
{
   balance::StrategyIndexed strategy(resourceList);

   IndexedTest::Identifier id(0);

   std::shared_ptr<TestResource> myResource = TestResource::cast_copy(strategy.apply(id));
   ASSERT_EQ(0, myResource->getKey());

   if(true) {
      balance::GuardResourceList guard(resourceList);
      std::shared_ptr<TestResource> myResource = TestResource::cast(resourceList->at(guard, 0));
      myResource->setAvailable(false);
   }

   myResource = TestResource::cast_copy(strategy.apply(id));
   ASSERT_EQ(1, myResource->getKey());

   if(true) {
      balance::GuardResourceList guard(resourceList);
      for(auto ii = resourceList->resource_begin(guard), maxii = resourceList->resource_end(guard); ii != maxii; ++ ii) {
         std::shared_ptr<TestResource> myResource = TestResource::cast(ResourceList::resource(ii));
         myResource->setAvailable(false);
      }
   }

   ASSERT_THROW(strategy.apply(id), ResourceUnavailableException);
}

TEST_F(StrategyIndexedFixture, balance_quality)
{
   balance::StrategyIndexed strategy(resourceList);
   IndexedTest::CounterContainer counterContainer;

   for(int ii = 0; ii < IndexedTest::MaxResources * IndexedTest::MaxLoop; ++ ii){
       IndexedTest::Identifier id(ii);
       IndexedTest::incrementUse(counterContainer, strategy.apply(id));
   }

   ASSERT_EQ(IndexedTest::MaxResources, counterContainer.size());

   for(IndexedTest::counter_iterator ii = counterContainer.begin(), maxii = counterContainer.end(); ii != maxii; ++ ii) {
      ASSERT_EQ(IndexedTest::MaxLoop, ii->second);
   }
}

TEST(StrategyIndexedTest, empty_strategy)
{
   std::shared_ptr<coffee::balance::ResourceList> emptyList = std::make_shared<coffee::balance::ResourceList>("EmptyList");
   balance::StrategyIndexed strategy(emptyList);
    IndexedTest::Identifier id(0);
    ASSERT_THROW(strategy.apply(id), ResourceUnavailableException);
}

TEST_F( ResourceListFixture, balance_multithread)
{
   balance::StrategyIndexed strategy(resourceList);
   IndexedTest::CounterContainer counterContainer;
   std::mutex mutexContainer;

   std::thread t1(IndexedTest::parallel_work, std::ref(mutexContainer), std::ref(counterContainer), std::ref(strategy));
   std::thread t2(IndexedTest::parallel_work, std::ref(mutexContainer), std::ref(counterContainer), std::ref(strategy));
   std::thread t3(IndexedTest::parallel_work, std::ref(mutexContainer), std::ref(counterContainer), std::ref(strategy));

   t1.join();
   t2.join();
   t3.join();

   int sumUse = 0;

   for(auto& useCounter : counterContainer) {
      ASSERT_GT(useCounter.second, 0);
      sumUse += useCounter.second;
   }

   ASSERT_EQ(IndexedTest::MaxResources * IndexedTest::MaxLoop, sumUse);
   ASSERT_EQ(IndexedTest::MaxResources, counterContainer.size());

   for(IndexedTest::counter_iterator ii = counterContainer.begin(), maxii = counterContainer.end(); ii != maxii; ++ ii) {
      ASSERT_EQ(IndexedTest::MaxLoop, ii->second);
   }
}

TEST_F(StrategyIndexedFixture, asXML)
{
   balance::StrategyIndexed strategy(resourceList);
   xml::Compiler compiler;
   auto root = std::make_shared<xml::Node>("root");
   ASSERT_EQ(676, compiler.apply(strategy.asXML(root)).size());
}
