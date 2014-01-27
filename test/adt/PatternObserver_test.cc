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

#include <wepa/adt/pattern/observer/Subject.hpp>
#include <wepa/adt/pattern/observer/Event.hpp>
#include <wepa/adt/pattern/observer/Observer.hpp>

#include <boost/test/unit_test.hpp>

using namespace wepa::adt;
using namespace wepa::adt::pattern;

const char* initialValue = "MyFirstValue";
const char* finalValue = "Cleared";

class TheSubject : public observer::Subject {
public:
   struct Events { enum _v { ChangeII = 0, ChangeSS, ChangeUU}; };

   TheSubject () : observer::Subject ("TheSubject") {
      m_ii = 0;
      m_uu = 0;
      m_ss = initialValue;

      m_events [Events::ChangeII] = registerEvent(Events::ChangeII);
      m_events [Events::ChangeSS] = registerEvent(Events::ChangeSS);
      m_events [Events::ChangeUU] = registerEvent(Events::ChangeUU);
   }

   void registerTooManyEvents () throw (RuntimeException) {
      for (int ii = 0; ii < 128; ++ ii)
         registerEvent(ii);
   }

   const observer::Event& getEvent (const Events::_v nn) const noexcept { return m_events [nn]; }

   void setII (const int ii) noexcept { m_ii = ii; notify(m_events [Events::ChangeII]); }
   void setSS (const std::string& ss) { m_ss = ss; notify(Events::ChangeSS); }
   void setUU (const unsigned int uu) noexcept { m_uu = uu; notify(m_events [Events::ChangeUU]); }

   int getII () const noexcept { return m_ii; }
   const std::string& getSS () const noexcept { return m_ss; }
   unsigned int getUU () const noexcept { return m_uu; }

   void clear () {
      m_ii = -1;
      m_ss = finalValue;
      m_uu = 0;
      notify ();
   }

private:
   observer::Event m_events [3];
   int m_ii;
   std::string m_ss;
   unsigned m_uu;
};

class TheObserverII : public observer::Observer {
public:
   TheObserverII (TheSubject& subject) :
      observer::Observer ("TheObserverII"), m_mySubject (subject), m_value (0)
   {
      subject.subscribeObserver(this, subject.getEvent(TheSubject::Events::ChangeII));
   }

   int getValue () const noexcept { return m_value; }

private:
   const TheSubject& m_mySubject;
   int m_value;

   void update (const observer::Event& event) noexcept {
      m_value = m_mySubject.getII ();
   }
};

class TheObserverSS : public observer::Observer {
public:
   TheObserverSS (TheSubject& subject) :
      observer::Observer ("TheObserverSS"), m_mySubject (subject)
   {
      subject.subscribeObserver(this, TheSubject::Events::ChangeSS);
   }

   const std::string& getValue () const noexcept { return m_value; }

private:
   const TheSubject& m_mySubject;
   std::string m_value;

   void update (const observer::Event& event) noexcept {
      m_value = m_mySubject.getSS ();
   }
};

class TheNosyObserver : public observer::Observer {
public:
   TheNosyObserver (TheSubject& subject) :
      observer::Observer ("TheNosyObserver"), m_mySubject (subject), m_ii (0), m_uu (0)
   {
      subject.subscribeObserver (this);
   }

   unsigned int getValue () const noexcept { return m_uu + m_ii; }

private:
   const TheSubject& m_mySubject;
   int m_ii;
   unsigned m_uu;

   void update (const observer::Event& event) noexcept {
      m_uu =  m_mySubject.getUU ();
      m_ii = m_mySubject.getII ();
   }
};

class DualObserver : public observer::Observer {
public:
   DualObserver (TheSubject& subject) :
      observer::Observer ("DualObserver"), m_mySubject (subject), m_ii (0), m_uu (0)
   {
      auto ev1 = subject.getEvent(TheSubject::Events::ChangeII);
      auto ev2 = subject.getEvent(TheSubject::Events::ChangeUU);
      subject.subscribeObserver (this);
   }

   int getII () const noexcept { return m_ii; }
   unsigned int getUU () const noexcept { return m_uu; }

private:
   const TheSubject& m_mySubject;
   int m_ii;
   unsigned m_uu;

   void update (const observer::Event& event) noexcept {
      m_uu =  m_mySubject.getUU ();
      m_ii = m_mySubject.getII ();
   }
};

class DullObserver : public observer::Observer {
public:
   DullObserver () : observer::Observer ("DullObserver") {;}
private:
   void update (const observer::Event& event) noexcept {
   }
};

BOOST_AUTO_TEST_CASE (successful_register)
{
   TheSubject subject;

   BOOST_REQUIRE_EQUAL (subject.getEvent (TheSubject::Events::ChangeII).getId (), 0);
   BOOST_REQUIRE_EQUAL (subject.getEvent (TheSubject::Events::ChangeII).getBitMask(), 1);
   BOOST_REQUIRE_EQUAL (subject.getEvent (TheSubject::Events::ChangeSS).getId (), 1);
   BOOST_REQUIRE_EQUAL (subject.getEvent (TheSubject::Events::ChangeSS).getBitMask(), 2);
   BOOST_REQUIRE_EQUAL (subject.getEvent (TheSubject::Events::ChangeUU).getId (), 2);
   BOOST_REQUIRE_EQUAL (subject.getEvent (TheSubject::Events::ChangeUU).getBitMask(), 4);
}

BOOST_AUTO_TEST_CASE (subscription_by_event)
{
   TheSubject subject;
   TheObserverII iiObserver (subject);

   BOOST_REQUIRE_EQUAL (iiObserver.getValue (), 0);

   subject.setII (100);

   BOOST_REQUIRE_EQUAL (iiObserver.getValue (), 100);

}

BOOST_AUTO_TEST_CASE (subscription_by_eventId)
{
   TheSubject subject;
   TheObserverSS ssObserver (subject);

   BOOST_REQUIRE_EQUAL (ssObserver.getValue (), initialValue);

   subject.setSS ("Wow!");

   BOOST_REQUIRE_EQUAL (ssObserver.getValue (), "Wow!");
}

BOOST_AUTO_TEST_CASE (nosy_subscription)
{
   TheSubject subject;
   TheObserverII iiObserver (subject);
   TheNosyObserver nosyObserver (subject);

   subject.setII (100);

   BOOST_REQUIRE_EQUAL (iiObserver.getValue (), 100);
   BOOST_REQUIRE_EQUAL (nosyObserver.getValue (), 100);

   subject.setUU (10);

   BOOST_REQUIRE_EQUAL (iiObserver.getValue (), 100);
   BOOST_REQUIRE_EQUAL (nosyObserver.getValue (), 110);
}

BOOST_AUTO_TEST_CASE (full_update)
{
   TheSubject subject;
   TheObserverII iiObserver (subject);
   TheObserverSS ssObserver (subject);
   TheNosyObserver nosyObserver (subject);

   BOOST_REQUIRE_EQUAL (ssObserver.getValue (), initialValue);

   subject.setII (100);

   BOOST_REQUIRE_EQUAL (iiObserver.getValue (), 100);
   BOOST_REQUIRE_EQUAL (nosyObserver.getValue (), 100);

   subject.setUU (10);

   BOOST_REQUIRE_EQUAL (iiObserver.getValue (), 100);
   BOOST_REQUIRE_EQUAL (nosyObserver.getValue (), 110);

   subject.clear ();

   BOOST_REQUIRE_EQUAL (iiObserver.getValue (), -1);
   BOOST_REQUIRE_EQUAL (ssObserver.getValue (), finalValue);
   BOOST_REQUIRE_EQUAL (nosyObserver.getValue (), 4294967295U);
}


BOOST_AUTO_TEST_CASE (dual_subscription)
{
   TheSubject subject;
   DualObserver the_observer (subject);

   subject.setII (100);
   subject.setUU(3000);

   BOOST_REQUIRE_EQUAL (the_observer.getII(), 100);
   BOOST_REQUIRE_EQUAL (the_observer.getUU(), 3000);

   subject.setUU (10003000);
   BOOST_REQUIRE_EQUAL (the_observer.getUU(), 10003000);
}

BOOST_AUTO_TEST_CASE (too_many_events)
{
   TheSubject subject;

   BOOST_REQUIRE_THROW (subject.registerTooManyEvents(), wepa::adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE (promicuous_observer)
{
   TheSubject subject1;
   TheSubject subject2;

   DualObserver theObserver (subject1);

   BOOST_REQUIRE_THROW (subject2.subscribeObserver(&theObserver), wepa::adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE (observer_out_of_scope)
{
   TheSubject subject;

   TheObserverII iiObserver (subject);

   if (true) {
      DualObserver dualObserver (subject);

      BOOST_REQUIRE_EQUAL(subject.countObservers(), 2);

      subject.setII (1010);
      subject.setUU (3333);

      BOOST_REQUIRE_EQUAL (dualObserver.getII(), 1010);
      BOOST_REQUIRE_EQUAL (dualObserver.getUU(), 3333);

      BOOST_REQUIRE_EQUAL (iiObserver.getValue(), 1010);
   }

   BOOST_REQUIRE_EQUAL(subject.countObservers(), 1);

   subject.setII (2020);

   BOOST_REQUIRE_EQUAL (iiObserver.getValue(), 2020);
}

