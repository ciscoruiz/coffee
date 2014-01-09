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
#include <wepa/balance/ByRange.hpp>
#include <wepa/balance/RoundRobin.hpp>

#include <wepa/xml/Node.hpp>
#include <wepa/xml/Compiler.hpp>

#include "TestResource.hpp"

using namespace wepa;
using namespace wepa::balance;
using namespace wepa::test::balance;

namespace ByRangeTest {
   typedef std::map <const Resource*, int> CounterContainer;
   typedef CounterContainer::iterator counter_iterator;

   void incrementUse (CounterContainer& container, const Resource* resource);
   void do_work (std::mutex& mutexContainer, CounterContainer& container, balance::BalanceIf& theBalance);
}

void ByRangeTest::incrementUse (CounterContainer& counterContainer, const Resource* resource)
{
   counter_iterator cc = counterContainer.find (resource);

   if (cc == counterContainer.end ()) {
      counterContainer [resource] = 1;
   }
   else {
      cc->second ++;
   }
}

void ByRangeTest::do_work(std::mutex& mutex, CounterContainer& counterContainer, BalanceIf& theBalance)
{
   for (int ii = 100; ii < 100 + 10; ++ ii) {
      Resource* resource = theBalance.apply (ii);

      if (true) {
         std::lock_guard <std::mutex> guard (mutex);
         incrementUse (counterContainer, resource);
      }
   }
}

BOOST_AUTO_TEST_CASE(byrange_requires_key)
{
   using namespace ByRangeTest;

   balance::ByRange myBalance;

   myBalance.initialize();

   BOOST_REQUIRE_THROW (myBalance.apply (), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(byrange_preconditions)
{
   using namespace ByRangeTest;

   balance::ByRange myBalance;

   myBalance.initialize();

   BOOST_REQUIRE_THROW (myBalance.addRange (0, 10, NULL), adt::RuntimeException);

   balance::RoundRobin empty;
   BOOST_REQUIRE_THROW (myBalance.addRange (0, 10, &empty), adt::RuntimeException);

   BOOST_REQUIRE_THROW (myBalance.addRange (0, 10, &myBalance), adt::RuntimeException);

   TestResource resource (100);
   balance::RoundRobin aux;
   aux.add (&resource);
   BOOST_REQUIRE_THROW (myBalance.addRange (10, 0, &aux), adt::RuntimeException);

   myBalance.addRange (10, 20, &aux);
   BOOST_REQUIRE_THROW (myBalance.addRange (15, 25, &aux), adt::RuntimeException);
   BOOST_REQUIRE_THROW (myBalance.addRange (5, 14, &aux), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(byrange_sharing)
{
   boost::ptr_vector <TestResource> resources;

   for (int ii = 0; ii < 5; ++ ii)
      resources.push_back (new TestResource (ii));

   balance::ByRange myBalance;

   balance::RoundRobin rr0;
   rr0.add (&resources [0]);
   rr0.add (&resources [1]);
   rr0.add (&resources [2]);
   myBalance.addRange(100, 200, &rr0);

   balance::RoundRobin rr1;
   rr1.add (&resources [2]);
   rr1.add (&resources [3]);
   myBalance.addRange(205, 350, &rr1);

   balance::RoundRobin rr2;
   rr2.add (&resources [3]);
   rr2.add (&resources [4]);
   myBalance.addRange(351, 450, &rr2);

   myBalance.initialize();

   BOOST_REQUIRE_EQUAL(myBalance.size (), 5);
   BOOST_REQUIRE_EQUAL(myBalance.countAvailableResources(), 5);

   BOOST_REQUIRE_EQUAL (myBalance.apply (120), &resources [0]);
   BOOST_REQUIRE_EQUAL (myBalance.apply (220), &resources [2]);
   BOOST_REQUIRE_EQUAL (myBalance.apply (420), &resources [3]);

   BOOST_REQUIRE_EQUAL (myBalance.apply (125), &resources [1]);
   BOOST_REQUIRE_EQUAL (myBalance.apply (225), &resources [3]);
   BOOST_REQUIRE_EQUAL (myBalance.apply (425), &resources [4]);

   BOOST_REQUIRE_EQUAL (myBalance.apply (127), &resources [2]);
   BOOST_REQUIRE_EQUAL (myBalance.apply (227), &resources [2]);
   BOOST_REQUIRE_EQUAL (myBalance.apply (427), &resources [3]);

   BOOST_REQUIRE_THROW (myBalance.apply (), adt::RuntimeException);
   BOOST_REQUIRE_THROW (myBalance.apply (2000), adt::RuntimeException);
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
