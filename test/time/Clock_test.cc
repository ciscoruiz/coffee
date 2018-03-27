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

#include <boost/test/unit_test.hpp>

#include <condition_variable>
#include <chrono>

#include <coffee/basis/pattern/observer/Observer.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TtyWriter.hpp>

#include <coffee/time/Clock.hpp>
#include <coffee/time/TimeService.hpp>
#include <coffee/time/TimeEvent.hpp>

#include <coffee/app/ApplicationServiceStarter.hpp>

#include "TimeFixture.hpp"

using namespace coffee;

using std::chrono::milliseconds;

using Subject = coffee::basis::pattern::observer::Subject;
using Event = coffee::basis::pattern::observer::Event;

struct ClockFixture : public TimeFixture {
   static const milliseconds MaxShortDuration;
   static const milliseconds ShortResolution;

   ClockFixture() : TimeFixture(MaxShortDuration, ShortResolution) {;}
};

const milliseconds ClockFixture::MaxShortDuration(1000);
const milliseconds ClockFixture::ShortResolution(50);

class ClockObserver : public basis::pattern::observer::Observer {
public:
   ClockObserver() :
      basis::pattern::observer::Observer("ClockObserver"),
      counter(0)
   {;}
   virtual ~ClockObserver() {;}

   bool receiveTicks(const int ticks, const milliseconds& maxWait) noexcept;

private:
   std::mutex mutex;
   std::condition_variable conditionForStop;
   int counter;

   void attached(const Subject& subject) noexcept { }
   void update(const Subject& subject, const Event& event) noexcept {
      std::unique_lock <std::mutex> guard (mutex);
      ++ counter;;
      conditionForStop.notify_one();
   }
   void detached(const Subject& subject) noexcept {  }
};

bool ClockObserver::receiveTicks(const int ticks, const milliseconds& maxWait)
   noexcept
{
   milliseconds ww(maxWait+ ClockFixture::ShortResolution);

   LOG_DEBUG("Waiting " << ticks << " | " << maxWait);

   std::unique_lock <std::mutex> guard(mutex);

   while(counter < ticks) {
      std::cv_status status = conditionForStop.wait_for(guard, ww);
      if (status == std::cv_status::timeout) {
         return false;
      }
   }

   return true;
}

BOOST_FIXTURE_TEST_CASE(clock_cancel_inactive, ClockFixture)
{
   auto clock = time::Clock::instantiate(111, milliseconds(200));
   BOOST_REQUIRE(!timeService->cancel(clock));
}

BOOST_FIXTURE_TEST_CASE(clock_cancel_empty, ClockFixture)
{
   std::shared_ptr<time::Clock> empty;
   BOOST_REQUIRE(!timeService->cancel(empty));
}

BOOST_FIXTURE_TEST_CASE(clock_bad_duration, ClockFixture)
{
   auto clock = time::Clock::instantiate(111, milliseconds(200));
   BOOST_REQUIRE_THROW(clock->getDuration(), basis::RuntimeException);
}

BOOST_FIXTURE_TEST_CASE(clock_basic, ClockFixture)
{
   auto observer = std::make_shared<ClockObserver>();
   timeService->attach(observer);

   auto clock = time::Clock::instantiate(111, milliseconds(200));
   BOOST_REQUIRE_NO_THROW(timeService->activate(clock));
   BOOST_CHECK(observer->receiveTicks(4, MaxShortDuration));
   BOOST_CHECK(timeService->cancel(clock));
}

BOOST_FIXTURE_TEST_CASE(clock_terminate_noempty, ClockFixture)
{
   auto clock = time::Clock::instantiate(111, milliseconds(200));
   BOOST_REQUIRE_NO_THROW(timeService->activate(clock));
   finalizeEmpty = false;
}
