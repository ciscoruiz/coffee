// WEPA - Write Excellent Professional Applications
//
//(c) Copyright 2013 Francisco Ruiz Rayo
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#include <wepa/adt/Millisecond.hpp>

#include <wepa/adt/pattern/observer/Subject.hpp>
#include <wepa/adt/pattern/observer/Event.hpp>
#include <wepa/adt/pattern/observer/Observer.hpp>

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace wepa::adt;
using namespace wepa::adt::pattern;

const char* initialValue = "MyFirstValue";
const char* finalValue = "Cleared";

class TheSubject : public observer::Subject {
public:
   struct Events { enum _v { ChangeII = 0, ChangeSS, ChangeUU}; };

   TheSubject() : observer::Subject("TheSubject") {
      m_ii = 0;
      m_uu = 0;
      m_ss = initialValue;

      m_events [Events::ChangeII] = registerEvent(Events::ChangeII);
      m_events [Events::ChangeSS] = registerEvent(Events::ChangeSS);
      m_events [Events::ChangeUU] = registerEvent(Events::ChangeUU);
   }

   void registerTooManyEvents() throw(RuntimeException) {
      for(int ii = 0; ii < 128; ++ ii)
         registerEvent(ii);
   }

   const observer::Event& getEvent(const Events::_v nn) const noexcept { return m_events [nn]; }

   void setII(const int ii) noexcept { m_ii = ii; notify(m_events [Events::ChangeII]); }
   void setSS(const std::string& ss) { m_ss = ss; notify(Events::ChangeSS); }
   void setUU(const unsigned int uu) noexcept { m_uu = uu; notify(m_events [Events::ChangeUU]); }

   int getII() const noexcept { return m_ii; }
   const std::string& getSS() const noexcept { return m_ss; }
   unsigned int getUU() const noexcept { return m_uu; }

   void clear() {
      m_ii = -1;
      m_ss = finalValue;
      m_uu = 0;
      notify(observer::Subject::AllEvents);
   }

private:
   observer::Event m_events [3];
   int m_ii;
   std::string m_ss;
   unsigned m_uu;
};

class TheObserverII : public observer::Observer {
public:
   TheObserverII() : observer::Observer("TheObserverII"), m_value(0) {}
   int getValue() const noexcept { return m_value; }

private:
   int m_value;

   void update(const observer::Subject& subject, const observer::Event& event) noexcept {
      m_value = static_cast<const TheSubject&>(subject).getII();
   }
};

class TheObserverSS : public observer::Observer {
public:
   TheObserverSS() : observer::Observer("TheObserverSS") {}

   const std::string& getValue() const noexcept { return m_value; }

private:
   std::string m_value;

   void update(const observer::Subject& subject, const observer::Event& event) noexcept {
      m_value = static_cast<const TheSubject&>(subject).getSS();
   }
};

class TheNosyObserver : public observer::Observer {
public:
   TheNosyObserver() : observer::Observer("TheNosyObserver"), m_ii(0), m_uu(0) {}

   unsigned int getValue() const noexcept { return m_uu + m_ii; }

private:
   int m_ii;
   unsigned m_uu;

   void update(const observer::Subject& subject, const observer::Event& event) noexcept {
      auto mySubject = static_cast<const TheSubject&>(subject);
      m_uu =  mySubject.getUU();
      m_ii = mySubject.getII();
   }
};

class OptimalObserver : public observer::Observer {
public:
   OptimalObserver() : observer::Observer("OptimalObserver") {}

   int getII() const noexcept { return m_ii.first; }
   const Millisecond& getIITime() { return m_ii.second; }

   unsigned int getUU() const noexcept { return m_uu.first; }
   const Millisecond& getUUTime() { return m_uu.second; }

private:
   pair<int, Millisecond> m_ii;
   pair<unsigned,Millisecond> m_uu;

   void update(const observer::Subject& subject, const observer::Event& event) noexcept {
      const TheSubject& mySubject = static_cast<const TheSubject&>(subject);

      if (event.getId() == TheSubject::Events::ChangeUU) {
         m_uu.first =  mySubject.getUU();
         m_uu.second = Millisecond::getTime();
      }
      if (event.getId() == TheSubject::Events::ChangeII) {
         m_ii.first = mySubject.getII();
         m_ii.second = Millisecond::getTime();
      }
   }
};

class NullObserver : public observer::Observer {
public:
   NullObserver() : observer::Observer("NullObserver") {;}
   void update(const observer::Subject& subject, const observer::Event& event) noexcept {}
};

BOOST_AUTO_TEST_CASE(successful_register)
{
   TheSubject subject;

   BOOST_REQUIRE_EQUAL(subject.getEvent(TheSubject::Events::ChangeII).getId(), 0);
   BOOST_REQUIRE_EQUAL(subject.getEvent(TheSubject::Events::ChangeII).getBitMask(), 1);
   BOOST_REQUIRE_EQUAL(subject.getEvent(TheSubject::Events::ChangeSS).getId(), 1);
   BOOST_REQUIRE_EQUAL(subject.getEvent(TheSubject::Events::ChangeSS).getBitMask(), 2);
   BOOST_REQUIRE_EQUAL(subject.getEvent(TheSubject::Events::ChangeUU).getId(), 2);
   BOOST_REQUIRE_EQUAL(subject.getEvent(TheSubject::Events::ChangeUU).getBitMask(), 4);
}

BOOST_AUTO_TEST_CASE(subscription_by_event)
{
   TheSubject subject;
   shared_ptr<TheObserverII> observer = make_shared<TheObserverII>();

   shared_ptr<observer::Observer> base = observer;
   subject.subscribeObserver(base, subject.getEvent(TheSubject::Events::ChangeII));

   BOOST_REQUIRE_EQUAL(observer->getValue(), 0);

   subject.setII(100);

   BOOST_REQUIRE_EQUAL(observer->getValue(), 100);
}

BOOST_AUTO_TEST_CASE(subscription_by_eventId)
{
   TheSubject subject;
   shared_ptr<TheObserverSS> observer= make_shared<TheObserverSS>();

   shared_ptr<observer::Observer> base = observer;
   subject.subscribeObserver(base, TheSubject::Events::ChangeSS);

   BOOST_REQUIRE_EQUAL(observer->getValue(), initialValue);

   subject.setSS("Wow!");

   BOOST_REQUIRE_EQUAL(observer->getValue(), "Wow!");
}

BOOST_AUTO_TEST_CASE(nosy_subscription)
{
   TheSubject subject;
   shared_ptr<TheObserverII> iiObserver = make_shared<TheObserverII>();

   shared_ptr<observer::Observer> base = iiObserver;
   subject.subscribeObserver(base, TheSubject::Events::ChangeSS);

   auto eventII = subject.getEvent(TheSubject::Events::ChangeII);
   auto eventUU = subject.getEvent(TheSubject::Events::ChangeUU);
   observer::Event event = eventII + eventUU;
   shared_ptr<TheNosyObserver> nosyObserver = make_shared<TheNosyObserver>();
   subject.subscribeObserver(base = nosyObserver, event);

   subject.setII(100);

   BOOST_REQUIRE_EQUAL(iiObserver->getValue(), 0);
   BOOST_REQUIRE_EQUAL(nosyObserver->getValue(), 100);

   subject.setUU(10);

   BOOST_REQUIRE_EQUAL(iiObserver->getValue(), 0);
   BOOST_REQUIRE_EQUAL(nosyObserver->getValue(), 110);
}

BOOST_AUTO_TEST_CASE(full_update)
{
   TheSubject subject;
   shared_ptr<TheObserverSS> ssObserver = make_shared<TheObserverSS>();
   shared_ptr<TheObserverII> iiObserver = make_shared<TheObserverII>();

   shared_ptr<observer::Observer> base = ssObserver;
   subject.subscribeObserver(base, TheSubject::Events::ChangeSS);

   subject.subscribeObserver(base = iiObserver, TheSubject::Events::ChangeII);

   auto eventII = subject.getEvent(TheSubject::Events::ChangeII);
   auto eventUU = subject.getEvent(TheSubject::Events::ChangeUU);
   observer::Event event = eventII + eventUU;
   shared_ptr<TheNosyObserver> nosyObserver = make_shared<TheNosyObserver>();
   subject.subscribeObserver(base = nosyObserver, event);

   BOOST_REQUIRE_EQUAL(ssObserver->getValue(), initialValue);

   subject.setII(100);

   BOOST_REQUIRE_EQUAL(iiObserver->getValue(), 100);
   BOOST_REQUIRE_EQUAL(nosyObserver->getValue(), 100);

   subject.setUU(50);

   BOOST_REQUIRE_EQUAL(iiObserver->getValue(), 100);
   BOOST_REQUIRE_EQUAL(nosyObserver->getValue(), 150);

   subject.clear();

   BOOST_REQUIRE_EQUAL(iiObserver->getValue(), -1);
   BOOST_REQUIRE_EQUAL(ssObserver->getValue(), finalValue);
   BOOST_REQUIRE_EQUAL(nosyObserver->getValue(), 4294967295U);
}

BOOST_AUTO_TEST_CASE(optimal_subscription)
{
   TheSubject subject;
   shared_ptr<OptimalObserver> observer = make_shared<OptimalObserver>();

   shared_ptr<observer::Observer> base = observer;
   auto eventII = subject.getEvent(TheSubject::Events::ChangeII);
   auto eventUU = subject.getEvent(TheSubject::Events::ChangeUU);
   observer::Event event = eventII + eventUU;
   subject.subscribeObserver(base, event);

   subject.setII(100);

   usleep(1100);

   subject.setUU(3000);

   BOOST_REQUIRE_EQUAL(observer->getII(), 100);
   BOOST_REQUIRE_EQUAL(observer->getUU(), 3000);
   BOOST_REQUIRE_LT(observer->getIITime(), observer->getUUTime());
}

BOOST_AUTO_TEST_CASE(too_many_events)
{
   TheSubject subject;

   BOOST_REQUIRE_THROW(subject.registerTooManyEvents(), wepa::adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(promicuous_observer)
{
   TheSubject subject1;
   TheSubject subject2;

   shared_ptr<NullObserver> observer = make_shared<NullObserver>();

   shared_ptr<observer::Observer> base = observer;
   subject1.subscribeObserver(base, TheSubject::Events::ChangeII);

   BOOST_REQUIRE_THROW(subject2.subscribeObserver(base, TheSubject::Events::ChangeII), wepa::adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(observer_out_of_scope)
{
   shared_ptr<NullObserver> observer = make_shared<NullObserver>();

   if (true) {
      TheSubject subject;

      shared_ptr<observer::Observer> base = observer;
      subject.subscribeObserver(base, TheSubject::Events::ChangeII);

      BOOST_REQUIRE_EQUAL(subject.countObservers(), 1);


      BOOST_REQUIRE_EQUAL(observer->isSubscribed(), true);
   }

   BOOST_REQUIRE_EQUAL(observer->isSubscribed(), false);
}

