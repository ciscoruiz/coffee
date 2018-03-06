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

#include <limits.h>
#include <signal.h>

#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <csignal>

#include <boost/test/unit_test.hpp>

#include <coffee/adt/Semaphore.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TtyWriter.hpp>

#include <coffee/app/ApplicationEngineRunner.hpp>
#include <coffee/app/Engine.hpp>

#include <coffee/xml/Document.hpp>
#include <coffee/xml/Node.hpp>

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
      semaphoreForRun(0),
      stopNow(false)
   {
      logger::Logger::initialize(std::make_shared<logger::TtyWriter>());
   }

   void waitForRun() { semaphoreForRun.wait(); }
   void terminateApplication() { stopNow = true; conditionForStop.notify_all(); }

private:
   adt::Semaphore semaphoreForRun;
   std::mutex mutex;
   std::condition_variable conditionForStop;
   bool stopNow;

   void run() throw(adt::RuntimeException) {
      semaphoreForRun.signal();
      std::unique_lock <std::mutex> guard (mutex);
      while(!stopNow) {
         conditionForStop.wait(guard);
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

   BOOST_REQUIRE(application.engine_find("00") != application.engine_end());

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

BOOST_AUTO_TEST_CASE( repeat_predecessor )
{
   SmallestApplication application;

   std::shared_ptr<MyEngine> engine = std::make_shared<MyEngine>(application, "00");
   engine->setPredecessor("SomeRepeatedName");
   engine->setPredecessor("SomeRepeatedName");

   BOOST_REQUIRE_NO_THROW(application.start());
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
   app::ApplicationEngineRunner application("app_already_run");

   auto thread = std::thread(parallelRun, std::ref(application));

   application.waitUntilRunning();
   BOOST_CHECK_THROW(application.start(), adt::RuntimeException);
   application.requestStop();
   thread.join();
}

BOOST_AUTO_TEST_CASE( app_stop_engines )
{
   app::ApplicationEngineRunner application("app_stop_engines");
   auto engine = std::make_shared<MyEngine>(application, "00");
   application.attach(engine);

   auto thread = std::thread(parallelRun, std::ref(application));
   application.waitUntilRunning();
   BOOST_CHECK(application.isRunning());

   BOOST_CHECK(engine->isRunning());
   BOOST_CHECK_EQUAL(engine->getInitializedCounter(), 1);
   BOOST_CHECK_EQUAL(engine->getStoppedCounter(), 0);

   application.requestStop();
   thread.join();

   BOOST_CHECK(engine->isStopped());
   BOOST_CHECK_EQUAL(engine->getInitializedCounter(), 1);
   BOOST_CHECK_EQUAL(engine->getStoppedCounter(), 1);

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
   app::ApplicationEngineRunner application("app_unstoppable_stop_engines");

   auto engine = std::make_shared<NoStopEngine>(application, "00");
   application.attach(engine);
   application.attach(std::make_shared<NoStopEngine>(application, "01"));
   application.attach(std::make_shared<NoStopEngine>(application, "02"));

   auto thread = std::thread(parallelRun, std::ref(application));
   application.waitUntilRunning();
   BOOST_CHECK(application.isRunning());

   application.requestStop();

   thread.join();

   BOOST_CHECK(application.isStopped());
   BOOST_CHECK(engine->isStopped());
}

BOOST_AUTO_TEST_CASE( app_unstoppable_requeststop_engines )
{
   app::ApplicationEngineRunner application("app_unstoppable_requeststop_engines");

   auto engine = std::make_shared<NoRequestStopEngine>(application, "00");
   application.attach(engine);
   application.attach(std::make_shared<NoRequestStopEngine>(application, "01"));
   application.attach(std::make_shared<NoRequestStopEngine>(application, "02"));

   auto thread = std::thread(parallelRun, std::ref(application));
   application.waitUntilRunning();
   BOOST_CHECK(application.isRunning());
   BOOST_CHECK(engine->isRunning());

   BOOST_CHECK_THROW(application.requestStop(), adt::RuntimeException);

   thread.join();

   BOOST_CHECK(application.isStopped());
   BOOST_CHECK(engine->isStopped());
}

BOOST_AUTO_TEST_CASE( engine_attach_after_running )
{
   app::ApplicationEngineRunner application("engine_attach_after_running");

   std::shared_ptr<MyEngine> engine = std::make_shared<MyEngine>(application, "00");
   application.attach(engine);

   auto thread = std::thread(parallelRun, std::ref(application));

   application.waitUntilRunning();
   BOOST_CHECK(application.isRunning());
   BOOST_CHECK(engine->isRunning());

   std::shared_ptr<MyEngine> secondEngine = std::make_shared<MyEngine>(application, "01");
   BOOST_CHECK(!secondEngine->isRunning());

   application.attach(secondEngine);
   BOOST_CHECK(secondEngine->isRunning());

   BOOST_CHECK_NO_THROW(application.requestStop());
   thread.join();
}

BOOST_AUTO_TEST_CASE( app_logger_level_change_onair )
{
   app::ApplicationEngineRunner application("app_logger_level_change_onair");

   std::shared_ptr<MyEngine> engine = std::make_shared<MyEngine>(application, "00");
   application.attach(engine);

   auto thread = std::thread(parallelRun, std::ref(application));

   logger::Logger::setLevel(logger::Level::Information);

   application.waitUntilRunning();
   BOOST_CHECK(application.isRunning());
   BOOST_CHECK(engine->isRunning());

   std::raise(SIGUSR2);
   BOOST_CHECK_EQUAL(logger::Logger::getLevel(), logger::Level::Debug);

   std::raise(SIGUSR2);
   BOOST_CHECK_EQUAL(logger::Logger::getLevel(), logger::Level::Error);

   std::raise(SIGUSR2);
   BOOST_CHECK_EQUAL(logger::Logger::getLevel(), logger::Level::Warning);

   BOOST_CHECK_NO_THROW(application.requestStop());
   thread.join();
}

struct CleanContextFilename {
   CleanContextFilename() : application("AppDownloadContext") {
      remove(application.getOutputContextFilename().c_str());

   }
   ~CleanContextFilename() {
      remove(application.getOutputContextFilename().c_str());
   }

   app::ApplicationEngineRunner application;
};

BOOST_FIXTURE_TEST_CASE( app_write_context, CleanContextFilename )
{
   application.attach(std::make_shared<MyEngine>(application, "00"));
   application.attach(std::make_shared<MyEngine>(application, "01"));

   auto thread = std::thread(parallelRun, std::ref(application));

   application.waitUntilRunning();
   BOOST_CHECK(application.isRunning());

   std::raise(SIGUSR1);

   xml::Document document;

   BOOST_CHECK_NO_THROW(document.parse(application.getOutputContextFilename()));

   auto root = document.getRoot();

   auto app = root->lookupChild("app.Application");
   auto engines = app->lookupChild("Engines");

   BOOST_CHECK_EQUAL(engines->childAt(0)->lookupChild("Runnable")->lookupAttribute("Name")->getValue(), "00");
   BOOST_CHECK_EQUAL(engines->childAt(1)->lookupChild("Runnable")->lookupAttribute("Name")->getValue(), "01");

   BOOST_CHECK_NO_THROW(application.requestStop());
   thread.join();
}

BOOST_FIXTURE_TEST_CASE( app_cannot_write_context, CleanContextFilename )
{
   application.setOutputContextFilename("/root/file-without-access.context");

   application.attach(std::make_shared<MyEngine>(application, "00"));
   application.attach(std::make_shared<MyEngine>(application, "01"));

   auto thread = std::thread(parallelRun, std::ref(application));

   application.waitUntilRunning();
   BOOST_CHECK(application.isRunning());

   std::raise(SIGUSR1);

   BOOST_CHECK_NO_THROW(application.requestStop());
   thread.join();

   BOOST_REQUIRE_THROW(boost::filesystem::exists(application.getOutputContextFilename()), std::runtime_error);
}

