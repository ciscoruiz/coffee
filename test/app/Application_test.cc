// COFFEE - COmpany eFFEEctive Platform
//
//(c) Copyright 2018 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/coffee
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
#include <limits.h>

#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>

#include <boost/test/unit_test.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TtyWriter.hpp>

#include <coffee/app/Application.hpp>
#include <coffee/app/Engine.hpp>

using namespace std;
using namespace coffee;

class SmallestApplication : public app::Application {
public:
   SmallestApplication() : app::Application("SmallerApplication", "This is the title", "1.0") {
      logger::Logger::initialize(std::make_shared<logger::TtyWriter>());
   }

   void run() throw(adt::RuntimeException) {;}
};

class ParallelApplication : public app::Application {
public:
   ParallelApplication() : app::Application("ParallelApplication", "It will wait till condition", "1.0"),
      stopNow(false)
   {
      logger::Logger::initialize(std::make_shared<logger::TtyWriter>());
   }

   void terminateApplication() { stopNow = true; waitForStop.notify_all(); }

private:
   std::mutex mutex;
   std::condition_variable waitForStop;
   bool stopNow;

   void run() throw(adt::RuntimeException) {
      std::unique_lock <std::mutex> guard (mutex);
      while(!stopNow) {
         waitForStop.wait(guard);
      }
   }

   void do_requestStop() throw(adt::RuntimeException) {
      try {
         app::Application::do_requestStop();
         terminateApplication();
      }
      catch(adt::RuntimeException&) {
         terminateApplication();
         throw;
      }
   }
};

void parallelRun(app::Application& app) {
   app.start();
}

class MyEngine : public app::Engine {
public:
   MyEngine(app::Application& application, const char* name) : Engine(application, name), m_initilized(0), m_stopped(0) {;}

   int getInitializedCounter() const noexcept { return m_initilized; }
   int getStoppedCounter() const noexcept { return m_stopped; }

   void setPredecessor(const char* predecessor) noexcept { this->addPredecessor(predecessor); }

private:
   int m_initilized;
   int m_stopped;

   void do_initialize() throw(adt::RuntimeException){ ++ m_initilized;}
   void do_stop() throw(adt::RuntimeException) { ++ m_stopped;}
};

class NoStopEngine : public app::Engine {
public:
   NoStopEngine(app::Application& application, const char* name) : app::Engine(application, name) {;}

private:
   void do_initialize() throw(adt::RuntimeException) {;}
   void do_stop() throw(adt::RuntimeException) { COFFEE_THROW_EXCEPTION("I dont want to stop"); }
};

class NoRequestStopEngine : public app::Engine {
public:
   NoRequestStopEngine(app::Application& application, const char* name) : app::Engine(application, name) {;}

private:
   void do_initialize() throw(adt::RuntimeException) {;}
   void do_requestStop() throw(adt::RuntimeException) { COFFEE_THROW_EXCEPTION("I dont accept request stop"); }
   void do_stop() throw(adt::RuntimeException) { ; }
};

BOOST_AUTO_TEST_CASE( smallest_application )
{ 
   SmallestApplication application;

   BOOST_REQUIRE_EQUAL(application.engine_find("00") == application.engine_end(), true);

   std::shared_ptr<MyEngine> engine = std::make_shared<MyEngine>(application, "00");
   application.attach(engine);
   application.start();

   BOOST_REQUIRE_EQUAL(application.getPid(), getpid());
   BOOST_REQUIRE_EQUAL(application.getVersion().find("1.0/"), 0);

   BOOST_REQUIRE_EQUAL(application.engine_find("00") != application.engine_end(), true);

   BOOST_REQUIRE_EQUAL(engine->getInitializedCounter(), 1);
   BOOST_REQUIRE_EQUAL(engine->getStoppedCounter(), 1);
}

BOOST_AUTO_TEST_CASE( status_application )
{
   SmallestApplication application;

   BOOST_REQUIRE_EQUAL(application.isStopped(), true);
   BOOST_REQUIRE_EQUAL(application.isRunning(), false);

   application.start();

   BOOST_REQUIRE_EQUAL(application.isStopped(), true);
   BOOST_REQUIRE_EQUAL(application.isRunning(), false);
}

BOOST_AUTO_TEST_CASE( undefined_predecessor )
{
   SmallestApplication application;

   std::shared_ptr<MyEngine> engine = std::make_shared<MyEngine>(application, "00");
   application.attach(engine);

   engine->setPredecessor("undefined");

   BOOST_REQUIRE_THROW(application.start(), adt::RuntimeException);;

   BOOST_REQUIRE_EQUAL(engine->getInitializedCounter(), 0);
   BOOST_REQUIRE_EQUAL(engine->getStoppedCounter(), 0);

   BOOST_REQUIRE_EQUAL(application.isStopped(), true);
   BOOST_REQUIRE_EQUAL(application.isRunning(), false);
}

BOOST_AUTO_TEST_CASE( interdependence_predecessor )
{
   SmallestApplication application;

   std::shared_ptr<MyEngine> engine00 = std::make_shared<MyEngine>(application, "00");
   application.attach(engine00);

   std::shared_ptr<MyEngine> engine01 = std::make_shared<MyEngine>(application, "01");
   application.attach(engine01);

   engine00->setPredecessor("01");
   engine01->setPredecessor("00");

   BOOST_REQUIRE_THROW(application.start(), adt::RuntimeException);;

   BOOST_REQUIRE_EQUAL(engine00->getInitializedCounter(), 0);
   BOOST_REQUIRE_EQUAL(engine00->getStoppedCounter(), 0);

   BOOST_REQUIRE_EQUAL(engine01->getInitializedCounter(), 0);
   BOOST_REQUIRE_EQUAL(engine01->getStoppedCounter(), 0);

   BOOST_REQUIRE_EQUAL(application.isStopped(), true);
   BOOST_REQUIRE_EQUAL(application.isRunning(), false);
}

BOOST_AUTO_TEST_CASE( iterator_engine )
{
   SmallestApplication application;

   application.attach(std::make_shared<MyEngine>(application, "00"));
   application.attach(std::make_shared<MyEngine>(application, "01"));
   application.attach(std::make_shared<MyEngine>(application, "02"));
   application.attach(std::make_shared<MyEngine>(application, "03"));
   application.attach(std::make_shared<MyEngine>(application, "04"));

   BOOST_REQUIRE_NO_THROW(application.start());

   const SmallestApplication& constApplication(application);

   auto ii = application.engine_begin();
   auto maxii = application.engine_end();

   app::Application::const_engine_iterator jj = constApplication.engine_begin();
   app::Application::const_engine_iterator  maxjj =  constApplication.engine_end();

   while (ii != maxii) {
	   BOOST_REQUIRE_EQUAL (app::Application::engine(ii)->asString(), app::Application::engine(jj)->asString());
	   BOOST_REQUIRE(jj != maxjj);
	   ++ ii;
	   ++ jj;
   }
}

BOOST_AUTO_TEST_CASE( app_already_run )
{
   ParallelApplication application;

   auto thread = std::thread(parallelRun, std::ref(application));

   usleep(5000);
   BOOST_CHECK_THROW(application.start(), adt::RuntimeException);
   application.requestStop();
   thread.join();
}

BOOST_AUTO_TEST_CASE( app_stop_engines )
{
   ParallelApplication application;
   auto engine = std::make_shared<MyEngine>(application, "00");
   application.attach(engine);

   auto thread = std::thread(parallelRun, std::ref(application));
   usleep(5000);
   BOOST_CHECK(application.isRunning());

   BOOST_CHECK(engine->isRunning());
   BOOST_CHECK_EQUAL(engine->getInitializedCounter(), 1);
   BOOST_CHECK_EQUAL(engine->getStoppedCounter(), 0);

   application.requestStop();

   usleep(5000);

   BOOST_CHECK(engine->isStopped());
   BOOST_CHECK_EQUAL(engine->getInitializedCounter(), 1);
   BOOST_CHECK_EQUAL(engine->getStoppedCounter(), 1);

   thread.join();
}

BOOST_AUTO_TEST_CASE( app_null_engine )
{
   std::shared_ptr<MyEngine> engine;
   SmallestApplication application;
   BOOST_REQUIRE_THROW(application.attach(engine), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE( app_already_defined )
{
   SmallestApplication application;
   application.attach(std::make_shared<MyEngine>(application, "00"));
   auto iiprev = application.engine_begin();
   auto iimaxprev = application.engine_end();

   application.attach(std::make_shared<MyEngine>(application, "00"));
   BOOST_REQUIRE(iiprev == application.engine_begin());
   BOOST_REQUIRE(iimaxprev == application.engine_end());
}

BOOST_AUTO_TEST_CASE( app_unstoppable_stop_engines )
{
   ParallelApplication application;

   auto engine = std::make_shared<NoStopEngine>(application, "00");
   application.attach(engine);
   application.attach(std::make_shared<NoStopEngine>(application, "01"));
   application.attach(std::make_shared<NoStopEngine>(application, "02"));

   auto thread = std::thread(parallelRun, std::ref(application));
   usleep(5000);
   BOOST_CHECK(application.isRunning());

   application.terminateApplication();

   thread.join();

   BOOST_CHECK(application.isStopped());
   BOOST_CHECK(engine->isStopped());
}

BOOST_AUTO_TEST_CASE( app_unstoppable_requeststop_engines )
{
   ParallelApplication application;

   auto engine = std::make_shared<NoRequestStopEngine>(application, "00");
   application.attach(engine);
   application.attach(std::make_shared<NoRequestStopEngine>(application, "01"));
   application.attach(std::make_shared<NoRequestStopEngine>(application, "02"));

   auto thread = std::thread(parallelRun, std::ref(application));
   usleep(50000);
   BOOST_CHECK(application.isRunning());
   BOOST_CHECK(engine->isRunning());

   BOOST_CHECK_THROW(application.requestStop(), adt::RuntimeException);

   thread.join();

   BOOST_CHECK(application.isStopped());
   BOOST_CHECK(engine->isStopped());
}


