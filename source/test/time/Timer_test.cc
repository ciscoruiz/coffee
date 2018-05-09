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
#include <coffee/basis/Average.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TtyWriter.hpp>

#include <coffee/time/Timer.hpp>
#include <coffee/time/TimeService.hpp>
#include <coffee/time/TimeEvent.hpp>

#include "TimeFixture.hpp"

using namespace coffee;
using std::chrono::milliseconds;

struct ShortTimeFixture : public TimeFixture {
   static const milliseconds MaxShortDuration;
   static const milliseconds ShortResolution;

   ShortTimeFixture() : TimeFixture(MaxShortDuration, ShortResolution) {;}
};

const milliseconds ShortTimeFixture::MaxShortDuration(1000);
const milliseconds ShortTimeFixture::ShortResolution(50);

struct LongTimeFixture : public TimeFixture {
   static const milliseconds MaxLongDuration;
   static const milliseconds LongResolution;

   LongTimeFixture() : TimeFixture(MaxLongDuration, LongResolution) {;}
};

const milliseconds LongTimeFixture::MaxLongDuration(5000);
const milliseconds LongTimeFixture::LongResolution(100);

using Subject = coffee::basis::pattern::observer::Subject;
using Event = coffee::basis::pattern::observer::Event;

class TimerObserver : public basis::pattern::observer::Observer {
public:
   TimerObserver() :
      basis::pattern::observer::Observer("TimeObserver"),
      avgDeviation("Deviation", "ms")
   {;}
   virtual ~TimerObserver() {;}

   bool receiveTimedouts(std::shared_ptr<coffee::time::TimeService>& timeService, const milliseconds& maxWait) noexcept;
   const basis::Average<milliseconds>& getAvgDeviation() const noexcept { return avgDeviation; }

private:
   std::mutex mutex;
   std::condition_variable conditionForStop;
   basis::Average<milliseconds> avgDeviation;

   void attached(const Subject& subject) noexcept { }
   void update(const Subject& subject, const Event& event) noexcept {
      std::unique_lock <std::mutex> guard(mutex);
      try {
         auto duration = static_cast<const time::TimeEvent&>(event).getDuration();
         auto timeout = static_cast<const time::TimeEvent&>(event).getTimeout();
         avgDeviation += duration - timeout;
      }
      catch(const basis::RuntimeException& ex) {
      }
      conditionForStop.notify_one();
   }
   void detached(const Subject& subject) noexcept {  }
};

bool TimerObserver::receiveTimedouts(std::shared_ptr<coffee::time::TimeService>& timeService, const milliseconds& maxWait)
   noexcept
{
   milliseconds ww(maxWait * 2);

   std::unique_lock <std::mutex> guard(mutex);

   while(!timeService->empty()) {
      std::cv_status status = conditionForStop.wait_for(guard, ww);
      if (status == std::cv_status::timeout) {
         return false;
      }
   }

   return true;
}

const milliseconds TimeFixture::time100ms(100);
const milliseconds TimeFixture::time200ms(200);

BOOST_FIXTURE_TEST_CASE(timer_first_case, ShortTimeFixture)
{
   BOOST_REQUIRE(timeService->isRunning());
}

BOOST_AUTO_TEST_CASE(timer_activate_service_stopped)
{
   app::ApplicationServiceStarter app("timer_activate_service_stopped");
   std::shared_ptr<time::TimeService> timeService = time::TimeService::instantiate(app, milliseconds(1000), milliseconds(200));
   BOOST_REQUIRE(app.isStopped());
   BOOST_REQUIRE(timeService->isStopped());
   auto timer = time::Timer::instantiate(100, milliseconds(100));
   BOOST_REQUIRE_THROW(timeService->activate(timer), basis::RuntimeException);
}

BOOST_FIXTURE_TEST_CASE(timer_empty, ShortTimeFixture)
{
   std::shared_ptr<time::Timer> emptyTimer;
   BOOST_REQUIRE_THROW(timeService->activate(emptyTimer), basis::RuntimeException);
}

BOOST_FIXTURE_TEST_CASE(timer_zero_timeout, ShortTimeFixture)
{
   auto timer = time::Timer::instantiate(100, milliseconds(0));
   BOOST_REQUIRE_THROW(timeService->activate(timer), basis::RuntimeException);
}

BOOST_FIXTURE_TEST_CASE(timer_over_maxtimeout, ShortTimeFixture)
{
   auto timer = time::Timer::instantiate(100, milliseconds(1500));
   BOOST_REQUIRE_THROW(timeService->activate(timer), basis::RuntimeException);
}

BOOST_FIXTURE_TEST_CASE(timer_below_resolution, ShortTimeFixture)
{
   auto timer = time::Timer::instantiate(100, milliseconds(10));
   BOOST_REQUIRE_THROW(timeService->activate(timer), basis::RuntimeException);
}

BOOST_AUTO_TEST_CASE(timer_bad_resolution_maxtime)
{
   app::ApplicationServiceStarter app("test_timer_bad_resolution_maxtime");
   std::shared_ptr<time::TimeService> timeService = time::TimeService::instantiate(app, milliseconds(1000), milliseconds(5555));
   BOOST_REQUIRE_THROW(app.start(), basis::RuntimeException);
}

BOOST_FIXTURE_TEST_CASE(timer_repeat_id, ShortTimeFixture)
{
   auto observer = std::make_shared<TimerObserver>();
   timeService->attach(observer);

   auto firstTimer = time::Timer::instantiate(3333, time100ms);
   BOOST_CHECK_NO_THROW(timeService->activate(firstTimer));

   auto secondTimer = time::Timer::instantiate(3333, time200ms);
   BOOST_REQUIRE_THROW(timeService->activate(secondTimer), basis::RuntimeException);

   BOOST_REQUIRE(observer->receiveTimedouts(timeService, time200ms));
}

BOOST_FIXTURE_TEST_CASE(timer_timeout, ShortTimeFixture)
{
   auto observer = std::make_shared<TimerObserver>();
   timeService->attach(observer);

   auto firstTimer = time::Timer::instantiate(1111, time100ms);
   BOOST_CHECK_NO_THROW(timeService->activate(firstTimer));

   auto secondTimer = time::Timer::instantiate(2222, time200ms);
   BOOST_CHECK_NO_THROW(timeService->activate(secondTimer));

   BOOST_REQUIRE(observer->receiveTimedouts(timeService, time200ms));
}

BOOST_FIXTURE_TEST_CASE(timer_max_timeout, ShortTimeFixture)
{
   auto observer = std::make_shared<TimerObserver>();
   timeService->attach(observer);

   auto maxTimer = time::Timer::instantiate(12345, MaxShortDuration);
   BOOST_CHECK_NO_THROW(timeService->activate(maxTimer));

   usleep(200000);
   auto firstTimer = time::Timer::instantiate(123, time100ms);
   BOOST_CHECK_NO_THROW(timeService->activate(firstTimer));

   usleep(200000);
   auto secondTimer = time::Timer::instantiate(1234, MaxShortDuration / 2);
   BOOST_CHECK_NO_THROW(timeService->activate(secondTimer));

   BOOST_REQUIRE(observer->receiveTimedouts(timeService, MaxShortDuration));
}

BOOST_FIXTURE_TEST_CASE(timer_populate, LongTimeFixture)
{
   auto observer = std::make_shared<TimerObserver>();
   timeService->attach(observer);

   for (int ii = 10000; ii < 10050; ++ ii) {
      const milliseconds timeout((rand() % MaxLongDuration.count()) / LongResolution.count() * LongResolution.count());
      if (timeout.count() > 0) {
         auto timer = time::Timer::instantiate(ii, timeout);
         BOOST_REQUIRE_NO_THROW(timeService->activate(timer));
         usleep(rand() % 200000);
      }
   }

   BOOST_REQUIRE(observer->receiveTimedouts(timeService, MaxLongDuration));

   auto avgDeviation = observer->getAvgDeviation();
   BOOST_REQUIRE(!avgDeviation.isEmpty());
   BOOST_REQUIRE_LE(avgDeviation.value().count(), LongResolution.count());
}

