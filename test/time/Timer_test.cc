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

#include <coffee/adt/pattern/observer/Observer.hpp>
#include <coffee/adt/Average.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TtyWriter.hpp>

#include <coffee/time/Timer.hpp>
#include <coffee/time/TimeService.hpp>
#include <coffee/time/TimeEvent.hpp>

#include <coffee/app/ApplicationServiceStarter.hpp>

using namespace coffee;

struct TimeFixture {
   static const adt::Millisecond time100ms;
   static const adt::Millisecond time200ms;

   app::ApplicationServiceStarter app;
   std::shared_ptr<time::TimeService> timeService;
   std::thread thr;

   TimeFixture(const adt::Millisecond& maxTime, const adt::Millisecond& resolution) : app("TimeFixture") {
      logger::Logger::setLevel(logger::Level::Local7);
      logger::Logger::initialize(logger::TtyWriter::instantiate());

      timeService = time::TimeService::instantiate(app, maxTime, resolution);
      thr = std::thread(parallelRun, std::ref(app));
      app.waitUntilRunning();
      timeService->waitUntilRunning();
   }

   virtual ~TimeFixture() {
      BOOST_CHECK(timeService->empty());
      app.stop();
      thr.join();
   }

   static void parallelRun(app::Application& app) {
      app.start();
   }
};

struct ShortTimeFixture : public TimeFixture {
   static const adt::Millisecond MaxShortDuration;
   static const adt::Millisecond ShortResolution;

   ShortTimeFixture() : TimeFixture(MaxShortDuration, ShortResolution) {;}
};

const adt::Millisecond ShortTimeFixture::MaxShortDuration(1000);
const adt::Millisecond ShortTimeFixture::ShortResolution(50);

struct LongTimeFixture : public TimeFixture {
   static const adt::Millisecond MaxLongDuration;
   static const adt::Millisecond LongResolution;

   LongTimeFixture() : TimeFixture(MaxLongDuration, LongResolution) {;}
};

const adt::Millisecond LongTimeFixture::MaxLongDuration(5000);
const adt::Millisecond LongTimeFixture::LongResolution(100);

using Subject = coffee::adt::pattern::observer::Subject;
using Event = coffee::adt::pattern::observer::Event;

class TimeObserver : public adt::pattern::observer::Observer {
public:
   TimeObserver() :
      adt::pattern::observer::Observer("TimeObserver"),
      avgDeviation("Deviation", "ms")
   {;}
   virtual ~TimeObserver() {;}

   bool receiveTimedouts(const int nevents, const adt::Millisecond& maxWait) noexcept;
   const adt::Average<adt::Millisecond>& getAvgDeviation() const noexcept { return avgDeviation; }

private:
   std::mutex mutex;
   std::condition_variable conditionForStop;
   adt::Average<adt::Millisecond> avgDeviation;

   void attached(const Subject& subject) noexcept { }
   void update(const Subject& subject, const Event& event) noexcept {
      std::unique_lock <std::mutex> guard (mutex);
      auto duration = static_cast<const time::TimeEvent&>(event).getDuration();
      auto timeout = static_cast<const time::TimeEvent&>(event).getTimeout();
      avgDeviation += duration - timeout;
      conditionForStop.notify_one();
   }
   void detached(const Subject& subject) noexcept {  }
};

bool TimeObserver::receiveTimedouts(const int nevents, const adt::Millisecond& maxWait) noexcept {
   std::unique_lock <std::mutex> guard (mutex);
   std::chrono::milliseconds ww(maxWait.getValue() + ShortTimeFixture::ShortResolution.getValue());

   LOG_DEBUG("Waiting " << nevents << " | " << maxWait.asString());
   const int maxSize = avgDeviation.size() + nevents;

   while(avgDeviation.size() != maxSize) {
      std::cv_status status = conditionForStop.wait_for(guard, ww);
      if (status == std::cv_status::timeout) {
         LOG_DEBUG(avgDeviation.asString());
         return avgDeviation.size() == nevents;
      }
   }

   return true;
}

const adt::Millisecond TimeFixture::time100ms(100);
const adt::Millisecond TimeFixture::time200ms(200);

BOOST_FIXTURE_TEST_CASE(timer_first_case, ShortTimeFixture)
{
   BOOST_REQUIRE(timeService->isRunning());
}

BOOST_AUTO_TEST_CASE(timer_activate_service_stopped)
{
   app::ApplicationServiceStarter app("timer_activate_service_stopped");
   std::shared_ptr<time::TimeService> timeService = time::TimeService::instantiate(app, adt::Millisecond(1000), adt::Millisecond(200));
   BOOST_REQUIRE(app.isStopped());
   BOOST_REQUIRE(timeService->isStopped());
   auto timer = time::Timer::instantiate(100, adt::Millisecond(100));
   BOOST_REQUIRE_THROW(timeService->activate(timer), adt::RuntimeException);
}

BOOST_FIXTURE_TEST_CASE(timer_empty, ShortTimeFixture)
{
   std::shared_ptr<time::Timer> emptyTimer;
   BOOST_REQUIRE_THROW(timeService->activate(emptyTimer), adt::RuntimeException);
}

BOOST_FIXTURE_TEST_CASE(timer_zero_timeout, ShortTimeFixture)
{
   auto timer = time::Timer::instantiate(100, adt::Millisecond(0));
   BOOST_REQUIRE_THROW(timeService->activate(timer), adt::RuntimeException);
}

BOOST_FIXTURE_TEST_CASE(timer_over_maxtimeout, ShortTimeFixture)
{
   auto timer = time::Timer::instantiate(100, adt::Millisecond(1500));
   BOOST_REQUIRE_THROW(timeService->activate(timer), adt::RuntimeException);
}

BOOST_FIXTURE_TEST_CASE(timer_below_resolution, ShortTimeFixture)
{
   auto timer = time::Timer::instantiate(100, adt::Millisecond(10));
   BOOST_REQUIRE_THROW(timeService->activate(timer), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(timer_bad_resolution_maxtime)
{
   app::ApplicationServiceStarter app("timer_bad_resolution_maxtime");
   std::shared_ptr<time::TimeService> timeService = time::TimeService::instantiate(app, adt::Millisecond(1000), adt::Millisecond(5555));
   BOOST_REQUIRE_THROW(app.start(), adt::RuntimeException);
}

BOOST_FIXTURE_TEST_CASE(timer_repeat_id, ShortTimeFixture)
{
   auto observer = std::make_shared<TimeObserver>();
   timeService->attach(observer);

   auto firstTimer = time::Timer::instantiate(3333, time100ms);
   BOOST_CHECK_NO_THROW(timeService->activate(firstTimer));

   auto secondTimer = time::Timer::instantiate(3333, time200ms);
   BOOST_REQUIRE_THROW(timeService->activate(secondTimer), adt::RuntimeException);

   BOOST_REQUIRE(observer->receiveTimedouts(1, time200ms));
}

BOOST_FIXTURE_TEST_CASE(timer_timeout, ShortTimeFixture)
{
   auto observer = std::make_shared<TimeObserver>();
   timeService->attach(observer);

   auto firstTimer = time::Timer::instantiate(1111, time100ms);
   BOOST_CHECK_NO_THROW(timeService->activate(firstTimer));

   auto secondTimer = time::Timer::instantiate(2222, time200ms);
   BOOST_CHECK_NO_THROW(timeService->activate(secondTimer));

   BOOST_REQUIRE(observer->receiveTimedouts(2, time200ms));
}

BOOST_FIXTURE_TEST_CASE(timer_max_timeout, ShortTimeFixture)
{
   auto observer = std::make_shared<TimeObserver>();
   timeService->attach(observer);

   auto maxTimer = time::Timer::instantiate(12345, MaxShortDuration);
   BOOST_CHECK_NO_THROW(timeService->activate(maxTimer));

   usleep(200000);
   auto firstTimer = time::Timer::instantiate(123, time100ms);
   BOOST_CHECK_NO_THROW(timeService->activate(firstTimer));

   usleep(200000);
   auto secondTimer = time::Timer::instantiate(1234, MaxShortDuration / 2);
   BOOST_CHECK_NO_THROW(timeService->activate(secondTimer));

   BOOST_REQUIRE(observer->receiveTimedouts(3, MaxShortDuration));
}

BOOST_FIXTURE_TEST_CASE(timer_populate, LongTimeFixture)
{
   auto observer = std::make_shared<TimeObserver>();
   timeService->attach(observer);

   for (int ii = 10000; ii < 10050; ++ ii) {
      const adt::Millisecond timeout ((rand() % MaxLongDuration) / LongResolution * LongResolution);
      if (timeout > 0) {
         auto timer = time::Timer::instantiate(ii, timeout);
         BOOST_REQUIRE_NO_THROW(timeService->activate(timer));
         usleep(rand() % 200000);
      }
   }

   BOOST_REQUIRE(observer->receiveTimedouts(timeService->size(), MaxLongDuration));

   auto avgDeviation = observer->getAvgDeviation();
   BOOST_REQUIRE(!avgDeviation.isEmpty());
   BOOST_REQUIRE_LE(avgDeviation.value(), LongResolution);
}
