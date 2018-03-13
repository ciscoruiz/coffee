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
#ifndef TEST_TIME_TIMEFIXTURE_HPP_
#define TEST_TIME_TIMEFIXTURE_HPP_

#include <coffee/adt/Millisecond.hpp>
#include <coffee/app/ApplicationServiceStarter.hpp>
#include <coffee/time/TimeService.hpp>

struct TimeFixture {
   static const coffee::adt::Millisecond time100ms;
   static const coffee::adt::Millisecond time200ms;

   coffee::app::ApplicationServiceStarter app;
   bool finalizeEmpty;
   std::shared_ptr<coffee::time::TimeService> timeService;
   std::thread thr;

   TimeFixture(const coffee::adt::Millisecond& maxTime, const coffee::adt::Millisecond& resolution) :
      app("TimeFixture"),
      finalizeEmpty(true)
   {
      coffee::logger::Logger::setLevel(coffee::logger::Level::Debug);
      coffee::logger::Logger::initialize(coffee::logger::TtyWriter::instantiate());

      timeService = coffee::time::TimeService::instantiate(app, maxTime, resolution);
      thr = std::thread(parallelRun, std::ref(app));
      app.waitUntilRunning();
      timeService->waitUntilRunning();
   }

   virtual ~TimeFixture() {
      BOOST_CHECK_EQUAL(timeService->empty(), finalizeEmpty);
      app.stop();
      thr.join();
   }

   static void parallelRun(coffee::app::Application& app) {
      app.start();
   }
};


#endif /* TEST_TIME_TIMEFIXTURE_HPP_ */
