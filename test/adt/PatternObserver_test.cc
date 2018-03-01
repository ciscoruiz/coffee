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

#include <coffee/adt/Millisecond.hpp>

#include <coffee/adt/pattern/observer/Subject.hpp>
#include <coffee/adt/pattern/observer/Event.hpp>
#include <coffee/adt/pattern/observer/Observer.hpp>

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace coffee::adt;
using namespace coffee::adt::pattern;

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

class ObserverUUAndII : public observer::Observer {
public:
   ObserverUUAndII() : observer::Observer("ObserverUUAndII"), m_ii(0), m_uu(0) {}

   unsigned int sumUpUUAndII() const noexcept { return m_uu + m_ii; }

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

   subject.attach(observer, subject.getEvent(TheSubject::Events::ChangeII));

   BOOST_REQUIRE_EQUAL(observer->getValue(), 0);

   subject.setII(100);

   BOOST_REQUIRE_EQUAL(observer->getValue(), 100);
}

BOOST_AUTO_TEST_CASE(subscription_by_eventId)
{
   TheSubject subject;
   shared_ptr<TheObserverSS> observer = make_shared<TheObserverSS>();

   subject.attach(observer, TheSubject::Events::ChangeSS);

   BOOST_REQUIRE_EQUAL(observer->getValue(), initialValue);

   subject.setSS("Wow!");

   BOOST_REQUIRE_EQUAL(observer->getValue(), "Wow!");
}

BOOST_AUTO_TEST_CASE(observer_detach)
{
   shared_ptr<NullObserver> observer = make_shared<NullObserver>();

   TheSubject subject;
   subject.attach(observer, TheSubject::Events::ChangeII);
   BOOST_REQUIRE(observer->isSubscribed() == true);

   BOOST_REQUIRE(subject.detach(observer->getName()) == true);
   BOOST_REQUIRE(observer->isSubscribed() == false);
}

BOOST_AUTO_TEST_CASE(observer_isnot_attached)
{
   TheSubject subject;
   BOOST_REQUIRE(subject.detach("undefinedObserver") == false);
}

BOOST_AUTO_TEST_CASE(observer_repeat_attach)
{
   shared_ptr<NullObserver> observer = make_shared<NullObserver>();

   TheSubject subject;
   subject.attach(observer, TheSubject::Events::ChangeII);
   BOOST_REQUIRE(subject.countObservers() == 1);

   subject.attach(observer, TheSubject::Events::ChangeII);
   BOOST_REQUIRE(subject.countObservers() == 1);
}

BOOST_AUTO_TEST_CASE(observer_undefined_event)
{
   shared_ptr<NullObserver> observer = make_shared<NullObserver>();

   TheSubject subject;
   BOOST_REQUIRE_THROW(subject.attach(observer, 10000), RuntimeException);
}

BOOST_AUTO_TEST_CASE(nosy_subscription)
{
   TheSubject subject;
   shared_ptr<TheObserverII> iiObserver = make_shared<TheObserverII>();

   subject.attach(iiObserver, TheSubject::Events::ChangeSS);

   auto eventII = subject.getEvent(TheSubject::Events::ChangeII);
   auto eventUU = subject.getEvent(TheSubject::Events::ChangeUU);
   observer::Event event = eventII + eventUU;
   shared_ptr<ObserverUUAndII> uuiiObserver = make_shared<ObserverUUAndII>();
   subject.attach(uuiiObserver, event);

   subject.setII(100);

   BOOST_REQUIRE_EQUAL(iiObserver->getValue(), 0);
   BOOST_REQUIRE_EQUAL(uuiiObserver->sumUpUUAndII(), 100);

   subject.setUU(10);

   BOOST_REQUIRE_EQUAL(iiObserver->getValue(), 0);
   BOOST_REQUIRE_EQUAL(uuiiObserver->sumUpUUAndII(), 110);
}

BOOST_AUTO_TEST_CASE(full_update)
{
   TheSubject subject;
   shared_ptr<TheObserverSS> ssObserver = make_shared<TheObserverSS>();
   shared_ptr<TheObserverII> iiObserver = make_shared<TheObserverII>();

   subject.attach(ssObserver, TheSubject::Events::ChangeSS);
   subject.attach(iiObserver, TheSubject::Events::ChangeII);

   auto eventII = subject.getEvent(TheSubject::Events::ChangeII);
   auto eventUU = subject.getEvent(TheSubject::Events::ChangeUU);
   observer::Event event = eventII + eventUU;
   shared_ptr<ObserverUUAndII> uuiiObserver = make_shared<ObserverUUAndII>();
   subject.attach(uuiiObserver, event);

   BOOST_REQUIRE_EQUAL(ssObserver->getValue(), initialValue);

   subject.setII(100);

   BOOST_REQUIRE_EQUAL(iiObserver->getValue(), 100);
   BOOST_REQUIRE_EQUAL(uuiiObserver->sumUpUUAndII(), 100);

   subject.setUU(50);

   BOOST_REQUIRE_EQUAL(iiObserver->getValue(), 100);
   BOOST_REQUIRE_EQUAL(uuiiObserver->sumUpUUAndII(), 150);

   subject.clear();

   BOOST_REQUIRE_EQUAL(iiObserver->getValue(), -1);
   BOOST_REQUIRE_EQUAL(ssObserver->getValue(), finalValue);
   BOOST_REQUIRE_EQUAL(uuiiObserver->sumUpUUAndII(), 4294967295U);
}

BOOST_AUTO_TEST_CASE(optimal_subscription)
{
   TheSubject subject;
   shared_ptr<OptimalObserver> observer = make_shared<OptimalObserver>();

   auto eventII = subject.getEvent(TheSubject::Events::ChangeII);
   auto eventUU = subject.getEvent(TheSubject::Events::ChangeUU);
   observer::Event event = eventII + eventUU;
   subject.attach(observer, event);

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

   BOOST_REQUIRE_THROW(subject.registerTooManyEvents(), coffee::adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(promicuous_observer)
{
   TheSubject subject1;
   TheSubject subject2;

   shared_ptr<NullObserver> observer = make_shared<NullObserver>();
   subject1.attach(observer, TheSubject::Events::ChangeII);

   BOOST_REQUIRE_THROW(subject2.attach(observer, TheSubject::Events::ChangeII), coffee::adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(observer_out_of_scope)
{
   shared_ptr<NullObserver> observer = make_shared<NullObserver>();

   if (true) {
      TheSubject subject;
      subject.attach(observer, TheSubject::Events::ChangeII);
      BOOST_REQUIRE_EQUAL(subject.countObservers(), 1);
      BOOST_REQUIRE_EQUAL(observer->isSubscribed(), true);
   }

   BOOST_REQUIRE_EQUAL(observer->isSubscribed(), false);
}
