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

#include <gtest/gtest.h>

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

struct TimerTestFixture : public TimeFixture {
   static const milliseconds MaxShortDuration;
   static const milliseconds ShortResolution;

   TimerTestFixture() : TimeFixture(MaxShortDuration, ShortResolution) {;}
};

const milliseconds TimerTestFixture::MaxShortDuration(1000);
const milliseconds TimerTestFixture::ShortResolution(50);

struct LongTimerTestFixture : public TimeFixture {
   static const milliseconds MaxLongDuration;
   static const milliseconds LongResolution;

   LongTimerTestFixture() : TimeFixture(MaxLongDuration, LongResolution) {;}
};

const milliseconds LongTimerTestFixture::MaxLongDuration(5000);
const milliseconds LongTimerTestFixture::LongResolution(100);

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

TEST(TimerTest, bad_resolution_maxtime)
{
   app::ApplicationServiceStarter app("test_timer_bad_resolution_maxtime");
   std::shared_ptr<time::TimeService> timeService = time::TimeService::instantiate(app, milliseconds(1000), milliseconds(5555));
   ASSERT_THROW(app.start(), basis::RuntimeException);
}

TEST(TimerTest, activate_while_service_stopped)
{
   app::ApplicationServiceStarter app("timer_activate_service_stopped");
   std::shared_ptr<time::TimeService> timeService = time::TimeService::instantiate(app, milliseconds(1000), milliseconds(200));
   ASSERT_TRUE(app.isStopped());
   ASSERT_TRUE(timeService->isStopped());
   auto timer = time::Timer::instantiate(100, milliseconds(100));
   ASSERT_THROW(timeService->activate(timer), basis::RuntimeException);
}

TEST_F(TimerTestFixture, time_service_running)
{
   ASSERT_TRUE(timeService->isRunning());
}

TEST_F(TimerTestFixture, empty_timer)
{
   std::shared_ptr<time::Timer> emptyTimer;
   ASSERT_THROW(timeService->activate(emptyTimer), basis::RuntimeException);
}

TEST_F(TimerTestFixture, zero_timeout)
{
   auto timer = time::Timer::instantiate(100, milliseconds(0));
   ASSERT_THROW(timeService->activate(timer), basis::RuntimeException);
}

TEST_F(TimerTestFixture, over_maxtimeout)
{
   auto timer = time::Timer::instantiate(100, milliseconds(1500));
   ASSERT_THROW(timeService->activate(timer), basis::RuntimeException);
}

TEST_F(TimerTestFixture, below_resolution)
{
   auto timer = time::Timer::instantiate(100, milliseconds(10));
   ASSERT_THROW(timeService->activate(timer), basis::RuntimeException);
}

TEST_F(TimerTestFixture, repeat_id)
{
   auto observer = std::make_shared<TimerObserver>();
   timeService->attach(observer);

   auto firstTimer = time::Timer::instantiate(3333, time100ms);
   EXPECT_NO_THROW(timeService->activate(firstTimer));

   auto secondTimer = time::Timer::instantiate(3333, time200ms);
   ASSERT_THROW(timeService->activate(secondTimer), basis::RuntimeException);

   ASSERT_TRUE(observer->receiveTimedouts(timeService, time200ms));
}

TEST_F(TimerTestFixture, timeout)
{
   auto observer = std::make_shared<TimerObserver>();
   timeService->attach(observer);

   auto firstTimer = time::Timer::instantiate(1111, time100ms);
   EXPECT_NO_THROW(timeService->activate(firstTimer));

   auto secondTimer = time::Timer::instantiate(2222, time200ms);
   EXPECT_NO_THROW(timeService->activate(secondTimer));

   ASSERT_TRUE(observer->receiveTimedouts(timeService, time200ms));
}

TEST_F(TimerTestFixture, max_timeout)
{
   auto observer = std::make_shared<TimerObserver>();
   timeService->attach(observer);

   auto maxTimer = time::Timer::instantiate(12345, MaxShortDuration);
   EXPECT_NO_THROW(timeService->activate(maxTimer));

   usleep(200000);
   auto firstTimer = time::Timer::instantiate(123, time100ms);
   EXPECT_NO_THROW(timeService->activate(firstTimer));

   usleep(200000);
   auto secondTimer = time::Timer::instantiate(1234, MaxShortDuration / 2);
   EXPECT_NO_THROW(timeService->activate(secondTimer));

   ASSERT_TRUE(observer->receiveTimedouts(timeService, MaxShortDuration));
}

TEST_F(LongTimerTestFixture, populate)
{
   auto observer = std::make_shared<TimerObserver>();
   timeService->attach(observer);

   for (int ii = 10000; ii < 10050; ++ ii) {
      const milliseconds timeout((rand() % MaxLongDuration.count()) / LongResolution.count() * LongResolution.count());
      if (timeout.count() > 0) {
         auto timer = time::Timer::instantiate(ii, timeout);
         ASSERT_NO_THROW(timeService->activate(timer));
         usleep(rand() % 200000);
      }
   }

   ASSERT_TRUE(observer->receiveTimedouts(timeService, MaxLongDuration));

   auto avgDeviation = observer->getAvgDeviation();
   ASSERT_TRUE(!avgDeviation.isEmpty());
   ASSERT_LE(avgDeviation.value().count(), LongResolution.count());
}

