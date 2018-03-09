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

#include <coffee/time/Timer.hpp>
#include <coffee/time/TimeService.hpp>

#include <coffee/app/ApplicationServiceStarter.hpp>

using namespace coffee;

struct TimeFixture {
   app::ApplicationServiceStarter app;
   std::shared_ptr<time::TimeService> timeService;
   std::thread thr;

   TimeFixture() : app("TimeFixture") {
      thr = std::thread(parallelRun, std::ref(app));
      timeService = time::TimeService::instantiate(app, adt::Millisecond(1000), adt::Millisecond(200));
      app.waitUntilRunning();
   }

   ~TimeFixture() {
      app.stop();
      thr.join();
   }

   static void parallelRun(app::Application& app) {
      app.start();
   }
};

BOOST_FIXTURE_TEST_CASE(timer_first_case, TimeFixture)
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

BOOST_FIXTURE_TEST_CASE(timer_empty, TimeFixture)
{
   std::shared_ptr<time::Timer> emptyTimer;
   BOOST_REQUIRE_THROW(timeService->activate(emptyTimer), adt::RuntimeException);
}

BOOST_FIXTURE_TEST_CASE(timer_zero_timeout, TimeFixture)
{
   auto timer = time::Timer::instantiate(100, adt::Millisecond(0));
   BOOST_REQUIRE_THROW(timeService->activate(timer), adt::RuntimeException);
}

BOOST_FIXTURE_TEST_CASE(timer_over_maxtimeout, TimeFixture)
{
   auto timer = time::Timer::instantiate(100, adt::Millisecond(1500));
   BOOST_REQUIRE_THROW(timeService->activate(timer), adt::RuntimeException);
}

BOOST_FIXTURE_TEST_CASE(timer_below_resolution, TimeFixture)
{
   auto timer = time::Timer::instantiate(100, adt::Millisecond(100));
   BOOST_REQUIRE_THROW(timeService->activate(timer), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(timer_bad_resolution_maxtime)
{
   app::ApplicationServiceStarter app("timer_bad_resolution_maxtime");
   std::shared_ptr<time::TimeService> timeService = time::TimeService::instantiate(app, adt::Millisecond(1000), adt::Millisecond(5000));
   BOOST_REQUIRE_THROW(app.start(), adt::RuntimeException);
}

