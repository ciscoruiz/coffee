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

#include <boost/test/unit_test.hpp>

#include <coffee/adt/AsString.hpp>

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

BOOST_AUTO_TEST_CASE(byrange_empty_strategy)
{
   balance::StrategyByRange mainStrategy;
   std::shared_ptr<balance::Strategy> foo;
   BOOST_REQUIRE_THROW (mainStrategy.addRange (0, 10, foo), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(byrange_recursive)
{
   std::shared_ptr<balance::StrategyByRange> strategy = std::make_shared<balance::StrategyByRange>();
   BOOST_REQUIRE_THROW (strategy->addRange (0, 10, strategy), adt::RuntimeException);
}

BOOST_FIXTURE_TEST_CASE(byrange_bad_limits, ResourceListFixture)
{
   balance::StrategyByRange mainStrategy;
   std::shared_ptr<balance::StrategyRoundRobin> strategy = std::make_shared<balance::StrategyRoundRobin>(resourceList);
   BOOST_REQUIRE_THROW (mainStrategy.addRange (10, 0, strategy), adt::RuntimeException);
}

BOOST_FIXTURE_TEST_CASE(byrange_overlapping, ResourceListFixture)
{
   balance::StrategyByRange mainStrategy;

   std::shared_ptr<balance::StrategyRoundRobin> strategy = std::make_shared<balance::StrategyRoundRobin>(resourceList);

   mainStrategy.addRange (10, 20, strategy);

   BOOST_REQUIRE_THROW (mainStrategy.addRange (5, 14, strategy), adt::RuntimeException);
   BOOST_REQUIRE_THROW (mainStrategy.addRange (5, 25, strategy), adt::RuntimeException);
   BOOST_REQUIRE_THROW (mainStrategy.addRange (15, 25, strategy), adt::RuntimeException);
   BOOST_REQUIRE_THROW (mainStrategy.addRange (11, 19, strategy), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(byrange_sharing)
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

   BOOST_REQUIRE_EQUAL(TestResource::cast(mainStrategy.apply(120))->getKey(), 0);
   BOOST_REQUIRE_EQUAL(TestResource::cast(mainStrategy.apply(220))->getKey(), 100);
   BOOST_REQUIRE_EQUAL(TestResource::cast(mainStrategy.apply(420))->getKey(), 1000);

   BOOST_REQUIRE_EQUAL(TestResource::cast(mainStrategy.apply(190))->getKey(), 1);
   BOOST_REQUIRE_EQUAL(TestResource::cast(mainStrategy.apply(300))->getKey(), 101);
   BOOST_REQUIRE_EQUAL(TestResource::cast(mainStrategy.apply(440))->getKey(), 1001);

   BOOST_REQUIRE_THROW(mainStrategy.apply (2000), balance::ResourceUnavailableException);
}
