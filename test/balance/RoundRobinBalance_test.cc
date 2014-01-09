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
#include <wepa/balance/RoundRobin.hpp>

#include <wepa/xml/Node.hpp>
#include <wepa/xml/Compiler.hpp>

#include "TestResource.hpp"

using namespace wepa;
using namespace wepa::balance;
using namespace wepa::test::balance;

namespace RoundRobinTest {
   typedef std::map <const Resource*, int> CounterContainer;
   typedef CounterContainer::iterator counter_iterator;

   void incrementUse (CounterContainer& container, const Resource* resource);
   void do_work (std::mutex& mutexContainer, CounterContainer& container, balance::BalanceIf& theBalance);
}

class MyRoundRobinBalance : public RoundRobin {
public:
   static const int MaxResources;
   static const int MaxLoop;

   MyRoundRobinBalance ();
   ~MyRoundRobinBalance () { m_resources.clear (); }

   TestResource* get (const int index) throw () { return &m_resources [index]; }

private:
   boost::ptr_vector<TestResource> m_resources;
};

const int MyRoundRobinBalance::MaxResources = 10;
const int MyRoundRobinBalance::MaxLoop = 3;

MyRoundRobinBalance::MyRoundRobinBalance () :
   balance::RoundRobin ()
{
   logger::Logger::initialize(new logger::TtyWriter);

   for (int ii = 0; ii < MyRoundRobinBalance::MaxResources; ++ ii) {
      TestResource* resource = new TestResource (ii);
      m_resources.push_back (resource);

      this->add (resource);
   }
}

void RoundRobinTest::incrementUse (CounterContainer& counterContainer, const Resource* resource)
{
   counter_iterator cc = counterContainer.find (resource);

   if (cc == counterContainer.end ()) {
      counterContainer [resource] = 1;
   }
   else {
      cc->second ++;
   }
}

void RoundRobinTest::do_work(std::mutex& mutex, CounterContainer& counterContainer, BalanceIf& theBalance)
{
   for (int ii = 0; ii < MyRoundRobinBalance::MaxResources; ++ ii) {
      Resource* resource = theBalance.apply ();

      if (true) {
         std::lock_guard <std::mutex> guard (mutex);
         incrementUse (counterContainer, resource);
      }
   }
}

BOOST_AUTO_TEST_CASE( rr_balance_quality)
{
   using namespace RoundRobinTest;

   MyRoundRobinBalance myBalance;

   BOOST_REQUIRE_THROW (myBalance.add (NULL), adt::RuntimeException);

   BOOST_REQUIRE_EQUAL(myBalance.add (myBalance.get (0)), false);

   myBalance.initialize();

   CounterContainer counterContainer;

   for (int ii = 0; ii < MyRoundRobinBalance::MaxResources * MyRoundRobinBalance::MaxLoop; ++ ii){
      incrementUse (counterContainer, myBalance.apply());
   }

   BOOST_REQUIRE_EQUAL (counterContainer.size (), MyRoundRobinBalance::MaxResources);

   for (counter_iterator ii = counterContainer.begin (), maxii = counterContainer.end (); ii != maxii; ++ ii) {
      BOOST_REQUIRE_EQUAL (ii->second, MyRoundRobinBalance::MaxLoop);
   }
}

BOOST_AUTO_TEST_CASE( rr_balance_dont_use_unavailables )
{
   MyRoundRobinBalance myBalance;

   myBalance.initialize();

   Resource* resource = myBalance.apply();

   BOOST_REQUIRE_EQUAL (resource, myBalance.get (0));

   myBalance.get (1)->setAvailable(false);

   resource = myBalance.apply();

   BOOST_REQUIRE_EQUAL (resource, myBalance.get (2));

   for (int ii = 0; ii < MyRoundRobinBalance::MaxResources; ++ ii) {
      myBalance.get(ii)->setAvailable(false);
   }

   BOOST_REQUIRE_THROW (myBalance.apply(), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE( rr_balance_multithread )
{
   using namespace RoundRobinTest;

   MyRoundRobinBalance myBalance;
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

   BOOST_REQUIRE_EQUAL(sumUse, MyRoundRobinBalance::MaxResources * MyRoundRobinBalance::MaxLoop);
}
