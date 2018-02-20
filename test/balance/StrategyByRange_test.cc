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

using namespace coffee;
using namespace coffee::balance;
using namespace coffee::test::balance;

namespace ByRangeTest {
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

   void parallel_work(std::mutex& mutexContainer, CounterContainer& counterContainer, balance::StrategyByRange& strategy)
   {
      for (int ii = 0; ii < 1000; ++ ii) {
         auto resource = strategy.apply (ii);

         if (true) {
            std::lock_guard <std::mutex> guard (mutexContainer);
            incrementUse (counterContainer, resource);
         }
      }
   }
}

BOOST_AUTO_TEST_CASE(byrange_empty_strategy)
{
   balance::StrategyByRange mainStrategy;
   std::shared_ptr<balance::Strategy> foo;
   BOOST_REQUIRE_THROW (mainStrategy.addRange (0, 10, foo), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(byrange_recursive)
{
   std::shared_ptr<balance::StrategyByRange> mainStrategy = std::make_shared<balance::StrategyByRange>();
   std::shared_ptr<balance::Strategy> foo = std::static_pointer_cast<balance::Strategy>(mainStrategy);
   BOOST_REQUIRE_THROW (mainStrategy->addRange (0, 10, foo), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(byrange_bad_limits)
{
   balance::StrategyByRange mainStrategy;

   auto resourceList = coffee::test::balance::setup(ByRangeTest::MaxResources);
   std::shared_ptr<balance::StrategyRoundRobin> strategy = std::make_shared<balance::StrategyRoundRobin>(resourceList);
   std::shared_ptr<balance::Strategy> foo = std::static_pointer_cast<balance::Strategy>(strategy);
   BOOST_REQUIRE_THROW (mainStrategy.addRange (10, 0, foo), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(byrange_overlapping)
{
   balance::StrategyByRange mainStrategy;

   auto resourceList = coffee::test::balance::setup(ByRangeTest::MaxResources);
   std::shared_ptr<balance::StrategyRoundRobin> strategy = std::make_shared<balance::StrategyRoundRobin>(resourceList);
   std::shared_ptr<balance::Strategy> foo = std::static_pointer_cast<balance::Strategy>(strategy);

   mainStrategy.addRange (10, 20, foo);

   BOOST_REQUIRE_THROW (mainStrategy.addRange (5, 14, foo), adt::RuntimeException);
   BOOST_REQUIRE_THROW (mainStrategy.addRange (5, 25, foo), adt::RuntimeException);
   BOOST_REQUIRE_THROW (mainStrategy.addRange (15, 25, foo), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(byrange_sharing)
{
   balance::StrategyByRange mainStrategy;

   {
      auto resourceList = coffee::test::balance::setup(ByRangeTest::MaxResources);
      std::shared_ptr<balance::StrategyRoundRobin> strategy = std::make_shared<balance::StrategyRoundRobin>(resourceList);
      std::shared_ptr<balance::Strategy> foo = std::static_pointer_cast<balance::Strategy>(strategy);
      mainStrategy.addRange(100, 200, foo);
   }

   {
      auto resourceList = coffee::test::balance::setup(ByRangeTest::MaxResources, 100);
      std::shared_ptr<balance::StrategyRoundRobin> strategy = std::make_shared<balance::StrategyRoundRobin>(resourceList);
      std::shared_ptr<balance::Strategy> foo = std::static_pointer_cast<balance::Strategy>(strategy);
      mainStrategy.addRange(205, 350, foo);
   }

   {
      auto resourceList = coffee::test::balance::setup(ByRangeTest::MaxResources, 1000);
      std::shared_ptr<balance::StrategyRoundRobin> strategy = std::make_shared<balance::StrategyRoundRobin>(resourceList);
      std::shared_ptr<balance::Strategy> foo = std::static_pointer_cast<balance::Strategy>(strategy);
      mainStrategy.addRange(351, 450, foo);
   }

   BOOST_REQUIRE_EQUAL(TestResource::cast(mainStrategy.apply(120))->getKey(), 0);
   BOOST_REQUIRE_EQUAL(TestResource::cast(mainStrategy.apply(220))->getKey(), 100);
   BOOST_REQUIRE_EQUAL(TestResource::cast(mainStrategy.apply(420))->getKey(), 1000);

   BOOST_REQUIRE_EQUAL(TestResource::cast(mainStrategy.apply(190))->getKey(), 1);
   BOOST_REQUIRE_EQUAL(TestResource::cast(mainStrategy.apply(300))->getKey(), 101);
   BOOST_REQUIRE_EQUAL(TestResource::cast(mainStrategy.apply(440))->getKey(), 1001);

   BOOST_REQUIRE_THROW(mainStrategy.apply (2000), balance::ResourceUnavailableException);
}

/*

BOOST_AUTO_TEST_CASE( byrange_balance_quality)
{
   using namespace ByRangeTest;

   balance::ByRange myBalance;

   BOOST_REQUIRE_THROW (myBalance.add (NULL), adt::RuntimeException);

   BOOST_REQUIRE_EQUAL(myBalance.add (myBalance.get (0)), false);

   myBalance.initialize();

   CounterContainer counterContainer;

   for (int ii = 0; ii < balance::ByRange::MaxResources * balance::ByRange::MaxLoop; ++ ii){
      incrementUse (counterContainer, myBalance.apply(ii));
   }

   BOOST_REQUIRE_EQUAL (counterContainer.size (), balance::ByRange::MaxResources);

   for (counter_iterator ii = counterContainer.begin (), maxii = counterContainer.end (); ii != maxii; ++ ii) {
      BOOST_REQUIRE_EQUAL (ii->second, balance::ByRange::MaxLoop);
   }
}

BOOST_AUTO_TEST_CASE( byrange_balance_dont_use_unavailables )
{
   balance::ByRange myBalance;

   myBalance.initialize();

   Resource* resource1 = myBalance.apply(5);
   Resource* resource2 = myBalance.apply(15);

   BOOST_REQUIRE_EQUAL (resource1, resource2);

   myBalance.get (5)->setAvailable(false);
   myBalance.get (6)->setAvailable(false);

   resource1 = myBalance.apply(5);

   BOOST_REQUIRE_EQUAL (resource1, myBalance.get (7));

   for (int ii = 0; ii < balance::ByRange::MaxResources; ++ ii) {
      myBalance.get(ii)->setAvailable(false);
   }

   BOOST_REQUIRE_THROW (myBalance.apply(2), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE( byrange_balance_multithread )
{
   using namespace ByRangeTest;

   balance::ByRange myBalance;
   std::mutex mutexContainer;
   CounterContainer counterContainer;

   myBalance.initialize();

   std::thread t1(do_work, std::ref (mutexContainer), std::ref (counterContainer), std::ref (myBalance));
   std::thread t2(do_work, std::ref (mutexContainer), std::ref (counterContainer), std::ref (myBalance));
   std::thread t3(do_work, std::ref (mutexContainer), std::ref (counterContainer), std::ref (myBalance));

   t1.join ();
   t2.join ();
   t3.join ();

   int sumUse = 0;

   for (auto& useCounter : counterContainer) {
      BOOST_REQUIRE_GT (useCounter.second, 0);
      sumUse += useCounter.second;
   }

   BOOST_REQUIRE_EQUAL(sumUse, balance::ByRange::MaxResources * balance::ByRange::MaxLoop);
}
*/
