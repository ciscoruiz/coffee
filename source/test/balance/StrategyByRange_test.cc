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

#include <map>
#include <thread>
#include <mutex>

#include <boost/ptr_container/ptr_vector.hpp>

#include <gtest/gtest.h>

#include <coffee/basis/AsString.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TtyWriter.hpp>

#include <coffee/balance/Resource.hpp>
#include <coffee/balance/GuardResourceList.hpp>
#include <coffee/balance/StrategyByRange.hpp>
#include <coffee/balance/StrategyRoundRobin.hpp>

#include <coffee/xml/Node.hpp>
#include <coffee/xml/Compiler.hpp>

#include "TestResource.hpp"
#include "ResourceListFixture.hpp"

using namespace coffee;
using namespace coffee::balance;
using namespace coffee::test::balance;

TEST(StrategyByRangeTest, empty_strategy)
{
   balance::StrategyByRange mainStrategy;
   std::shared_ptr<balance::Strategy> foo;
   ASSERT_THROW (mainStrategy.addRange (0, 10, foo), basis::RuntimeException);
}

TEST(StrategyByRangeTest, byrange_recursive)
{
   std::shared_ptr<balance::StrategyByRange> strategy = std::make_shared<balance::StrategyByRange>();
   ASSERT_THROW (strategy->addRange (0, 10, strategy), basis::RuntimeException);
}

struct StrategyByRangeFixture : public ResourceListFixture {};

TEST_F(StrategyByRangeFixture, bad_limits)
{
   balance::StrategyByRange mainStrategy;
   std::shared_ptr<balance::StrategyRoundRobin> strategy = std::make_shared<balance::StrategyRoundRobin>(resourceList);
   ASSERT_THROW (mainStrategy.addRange (10, 0, strategy), basis::RuntimeException);
}

TEST_F(StrategyByRangeFixture, overlapping)
{
   balance::StrategyByRange mainStrategy;

   std::shared_ptr<balance::StrategyRoundRobin> strategy = std::make_shared<balance::StrategyRoundRobin>(resourceList);

   mainStrategy.addRange (10, 20, strategy);

   ASSERT_THROW (mainStrategy.addRange (5, 14, strategy), basis::RuntimeException);
   ASSERT_THROW (mainStrategy.addRange (5, 25, strategy), basis::RuntimeException);
   ASSERT_THROW (mainStrategy.addRange (15, 25, strategy), basis::RuntimeException);
   ASSERT_THROW (mainStrategy.addRange (11, 19, strategy), basis::RuntimeException);
}

class RequestId : public balance::Strategy::Request {
public:
    RequestId(const std::vector<int>& identifiers) : m_identifiers(identifiers),  m_ii(m_identifiers.begin()){;}
    int calculateIdentifier() const  noexcept {
       if (m_ii == m_identifiers.end())
          return 0;

       int result = *m_ii;
       ++m_ii;
       return result;
    }

private:
    const std::vector<int> m_identifiers;
    mutable std::vector<int>::const_iterator m_ii;
};

TEST(StrategyByRangeTest, sharing)
{
   balance::StrategyByRange mainStrategy;

   {
      auto resourceList = ResourceListFixture::setup(ResourceListFixture::MaxResources, 0);
      std::shared_ptr<balance::StrategyRoundRobin> strategy = std::make_shared<balance::StrategyRoundRobin>(resourceList);
      ASSERT_NO_THROW(mainStrategy.addRange(100, 200, strategy));
   }

   {
      auto resourceList = ResourceListFixture::setup(ResourceListFixture::MaxResources, 100);
      std::shared_ptr<balance::StrategyRoundRobin> strategy = std::make_shared<balance::StrategyRoundRobin>(resourceList);
      ASSERT_NO_THROW(mainStrategy.addRange(205, 350, strategy));
   }

   {
      auto resourceList = ResourceListFixture::setup(ResourceListFixture::MaxResources, 1000);
      std::shared_ptr<balance::StrategyRoundRobin> strategy = std::make_shared<balance::StrategyRoundRobin>(resourceList);
      ASSERT_NO_THROW(mainStrategy.addRange(351, 450, strategy));
   }

   std::vector<int> identifiers = { 120, 220, 420, 190, 300, 400, 2000 };
   RequestId requestId(identifiers);

   ASSERT_EQ(0, TestResource::cast(mainStrategy.apply(requestId))->getKey());
   ASSERT_EQ(100, TestResource::cast(mainStrategy.apply(requestId))->getKey());
   ASSERT_EQ(1000, TestResource::cast(mainStrategy.apply(requestId))->getKey());

   ASSERT_EQ(1, TestResource::cast(mainStrategy.apply(requestId))->getKey());
   ASSERT_EQ(101, TestResource::cast(mainStrategy.apply(requestId))->getKey());
   ASSERT_EQ(1001, TestResource::cast(mainStrategy.apply(requestId))->getKey());

   ASSERT_THROW(mainStrategy.apply (requestId), balance::ResourceUnavailableException);
}

TEST(StrategyByRangeTest, byrange_asXML)
{
   balance::StrategyByRange mainStrategy;

   {
      auto resourceList = ResourceListFixture::setup(ResourceListFixture::MaxResources, 0);
      std::shared_ptr<balance::StrategyRoundRobin> strategy = std::make_shared<balance::StrategyRoundRobin>(resourceList);
      mainStrategy.addRange(100, 200, strategy);
   }

   {
      auto resourceList = ResourceListFixture::setup(ResourceListFixture::MaxResources, 100);
      std::shared_ptr<balance::StrategyRoundRobin> strategy = std::make_shared<balance::StrategyRoundRobin>(resourceList);
      mainStrategy.addRange(205, 350, strategy);
   }

   {
      auto resourceList = ResourceListFixture::setup(ResourceListFixture::MaxResources, 1000);
      std::shared_ptr<balance::StrategyRoundRobin> strategy = std::make_shared<balance::StrategyRoundRobin>(resourceList);
      mainStrategy.addRange(351, 450, strategy);
   }

   auto root = std::make_shared<xml::Node>("root");
   auto asXML = mainStrategy.asXML(root);

   xml::Compiler compiler;

   ASSERT_EQ("<root><StrategyByRange><Ranges><Range Begin=\"100\" End=\"200\" Strategy=\"balance::RoundRobin\"/><Range Begin=\"205\" End=\"350\" Strategy=\"balance::RoundRobin\"/><Range Begin=\"351\" End=\"450\" Strategy=\"balance::RoundRobin\"/></Ranges></StrategyByRange></root>", compiler.apply(root));
}
