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
#include <boost/ptr_container/ptr_vector.hpp>

#include <boost/test/unit_test.hpp>

#include <wepa/adt/AsString.hpp>

#include <wepa/logger/Logger.hpp>
#include <wepa/logger/TtyWriter.hpp>

#include <wepa/balance/Resource.hpp>
#include <wepa/balance/BalanceIf.hpp>

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

class MyBaseBalance : public BalanceIf {
public:
   MyBaseBalance (const Requires::_v requires);
   ~MyBaseBalance () { m_resources.clear (); }

   MyResource* get (const int index) throw () { return &m_resources [index]; }

private:
   boost::ptr_vector<MyResource> m_resources;
};

class MyBasicBalance : public MyBaseBalance {
public:
   MyBasicBalance () : MyBaseBalance (Requires::None) {;}

private:
   Resource* do_apply (const int key) throw (adt::RuntimeException);
};

class MyKeyBalance : public MyBaseBalance {
public:
   MyKeyBalance () : MyBaseBalance (Requires::Key) {;}

private:
   Resource* do_apply (const int key) throw (adt::RuntimeException);
};

int MaxResources = 10;

MyBaseBalance::MyBaseBalance (const Requires::_v requires) :
   BalanceIf ("MyBalance", requires)
{
   logger::Logger::initialize(new logger::TtyWriter);

   for (int ii = 0; ii < MaxResources; ++ ii) {
      MyResource* resource = new MyResource (ii);
      m_resources.push_back (resource);

      this->add (resource);
   }
}

Resource* MyBasicBalance::do_apply (const int key)
   throw (adt::RuntimeException)
{
   for (resource_iterator ii = this->resource_begin(), maxii = this->resource_end(); ii != maxii; ++ ii) {
      Resource* resource = BalanceIf::resource (ii);

      if (resource->isAvailable() == false)
         continue;

      return resource;
   }

   return NULL;
}

Resource* MyKeyBalance::do_apply (const int key)
   throw (adt::RuntimeException)
{
   for (resource_iterator ii = this->resource_begin(), maxii = this->resource_end(); ii != maxii; ++ ii) {
      Resource* resource = BalanceIf::resource (ii);

      MyResource* myResource = static_cast <MyResource*> (resource);

      if (myResource->getKey () == key)
         return resource;
   }

   return NULL;
}

BOOST_AUTO_TEST_CASE( count_availables )
{
   MyBasicBalance myBalance;

   BOOST_REQUIRE_THROW (myBalance.add (NULL), adt::RuntimeException);

   BOOST_REQUIRE_EQUAL(myBalance.add (myBalance.get (0)), false);

   myBalance.initialize();

   BOOST_REQUIRE_EQUAL(myBalance.size (), MaxResources);
   BOOST_REQUIRE_EQUAL(myBalance.countAvailableResources(), MaxResources);

   for (int ii = 0; ii < MaxResources; ++ ii) {
      if ((ii % 2) == 0)
         myBalance.get(ii)->setAvailable(false);
   }

   BOOST_REQUIRE_EQUAL(myBalance.countAvailableResources(), MaxResources / 2);
}

BOOST_AUTO_TEST_CASE( dont_use_unavailables )
{
   MyBasicBalance myBalance;

   myBalance.initialize();

   Resource* resource = myBalance.apply();

   BOOST_REQUIRE_EQUAL (resource, myBalance.get (0));

   myBalance.get (0)->setAvailable(false);

   resource = myBalance.apply();

   BOOST_REQUIRE_EQUAL (resource, myBalance.get (1));

   for (int ii = 0; ii < MaxResources; ++ ii) {
      myBalance.get(ii)->setAvailable(false);
   }

   BOOST_REQUIRE_THROW (myBalance.apply(), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE (must_have_key)
{
   MyKeyBalance myBalance;

   myBalance.initialize();

   BOOST_REQUIRE_THROW (myBalance.apply(), adt::RuntimeException);

   Resource* resource = myBalance.apply(MaxResources / 2);

   BOOST_REQUIRE_EQUAL (resource, myBalance.get (MaxResources / 2));
}


BOOST_AUTO_TEST_CASE (avoid_using_unavailable)
{
   MyKeyBalance myBalance;

   myBalance.initialize();

   myBalance.get (MaxResources / 2)->setAvailable(false);

   BOOST_REQUIRE_THROW (myBalance.apply (MaxResources / 2), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE (as_string)
{
   MyKeyBalance myBalance;

   myBalance.initialize();

   myBalance.get (MaxResources / 2)->setAvailable(false);

   BOOST_REQUIRE_EQUAL (myBalance.asString (), "balance::BalanceIf {adt::NamedObject { Name: MyBalance } } | Available = 9 of 10}");
}

BOOST_AUTO_TEST_CASE (as_xml)
{
   MyKeyBalance myBalance;

   myBalance.initialize();

   myBalance.get (MaxResources / 2)->setAvailable(false);

   xml::Node root ("root");

   myBalance.asXML(root);

   xml::Compiler compiler;

   std::cout << compiler.apply(root) << std::endl;
}
