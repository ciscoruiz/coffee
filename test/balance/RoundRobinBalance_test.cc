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

#include <boost/ptr_container/ptr_vector.hpp>

#include <boost/test/unit_test.hpp>

#include <wepa/adt/AsString.hpp>

#include <wepa/logger/Logger.hpp>
#include <wepa/logger/TtyWriter.hpp>

#include <wepa/balance/Resource.hpp>
#include <wepa/balance/RoundRobin.hpp>

#include <wepa/xml/Node.hpp>
#include <wepa/xml/Compiler.hpp>

using namespace wepa;
using namespace wepa::balance;

class MyResource : public Resource {
public:
   MyResource (const int key) :
      Resource (adt::StreamString ("MyResource-").append (adt::AsString::apply (key, "%02d"))),
      m_key (key),
      m_available (false) {;}

   void setAvailable (const bool available) throw () { m_available = available; }
   int getKey () const throw () { return m_key; }

private:
   const int m_key;
   bool m_available;

   bool isAvailable () const throw () { return m_available; }
   void initialize () throw (adt::RuntimeException) { m_available = true; }
};

class MyRoundRobinBalance : public RoundRobin {
public:
   static const int MaxResources;

   MyRoundRobinBalance ();
   ~MyRoundRobinBalance () { m_resources.clear (); }

   MyResource* get (const int index) throw () { return &m_resources [index]; }

private:
   boost::ptr_vector<MyResource> m_resources;
};

MyRoundRobinBalance::MyRoundRobinBalance () :
   balance::RoundRobin ()
{
   logger::Logger::initialize(new logger::TtyWriter);

   for (int ii = 0; ii < MyRoundRobinBalance::MaxResources; ++ ii) {
      MyResource* resource = new MyResource (ii);
      m_resources.push_back (resource);

      this->add (resource);
   }
}

const int MyRoundRobinBalance::MaxResources = 10;

BOOST_AUTO_TEST_CASE( rr_balance_quality)
{
   MyRoundRobinBalance myBalance;
   typedef std::map <Resource*, int> CounterContainer;
   typedef CounterContainer::iterator counter_iterator;

   BOOST_REQUIRE_THROW (myBalance.add (NULL), adt::RuntimeException);

   BOOST_REQUIRE_EQUAL(myBalance.add (myBalance.get (0)), false);

   myBalance.initialize();

   CounterContainer counterContainer;

   const int maxLoop = 3;

   for (int ii = 0; ii < MyRoundRobinBalance::MaxResources * maxLoop; ++ ii){
      Resource* resource = myBalance.apply();

      counter_iterator cc = counterContainer.find (resource);

      if (cc == counterContainer.end ()) {
         counterContainer [resource] = 1;
      }
      else {
         cc->second ++;
      }
   }

   BOOST_REQUIRE_EQUAL (counterContainer.size (), MyRoundRobinBalance::MaxResources);

   for (counter_iterator ii = counterContainer.begin (), maxii = counterContainer.end (); ii != maxii; ++ ii) {
      BOOST_REQUIRE_EQUAL (ii->second, maxLoop);
   }
}

BOOST_AUTO_TEST_CASE( rr_dont_use_unavailables )
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
