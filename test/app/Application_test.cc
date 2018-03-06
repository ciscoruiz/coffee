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
#include <coffee/app/ApplicationServiceStarter.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TtyWriter.hpp>

#include <coffee/app/Service.hpp>
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

void parallelRun(app::Application& app) {
   app.start();
}

class MyService : public app::Service {
public:
   MyService(app::Application& application, const char* name) : Service(application, name), m_initilized(0), m_stopped(0) {;}

   int getInitializedCounter() const noexcept { return m_initilized; }
   int getStoppedCounter() const noexcept { return m_stopped; }

   void setPredecessor(const char* predecessor) noexcept { this->addPredecessor(predecessor); }

private:
   int m_initilized;
   int m_stopped;

   void do_initialize() throw(adt::RuntimeException);
   void do_stop() throw(adt::RuntimeException);
};

void MyService::do_initialize() throw(adt::RuntimeException){ ++ m_initilized;}
void MyService::do_stop() throw(adt::RuntimeException) { ++ m_stopped;}

class NoStopService : public app::Service {
public:
   NoStopService(app::Application& application, const char* name) : app::Service(application, name) {;}

private:
   void do_initialize() throw(adt::RuntimeException);
   void do_stop() throw(adt::RuntimeException);
};

void NoStopService::do_initialize() throw(adt::RuntimeException) {;}

void NoStopService::do_stop()
   throw(adt::RuntimeException)
{
   COFFEE_THROW_EXCEPTION("I dont want to stop");
}

BOOST_AUTO_TEST_CASE( smallest_application )
{ 
   SmallestApplication application;

   BOOST_REQUIRE_EQUAL(application.service_find("00") == application.service_end(), true);

   std::shared_ptr<MyService> service = std::make_shared<MyService>(application, "00");
   application.attach(service);
   application.start();

   BOOST_REQUIRE_EQUAL(application.getPid(), getpid());
   BOOST_REQUIRE_EQUAL(application.getVersion().find("1.0/"), 0);

   BOOST_REQUIRE(application.service_find("00") != application.service_end());

   BOOST_REQUIRE_EQUAL(service->getInitializedCounter(), 1);
   BOOST_REQUIRE_EQUAL(service->getStoppedCounter(), 1);
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

   std::shared_ptr<MyService> service = std::make_shared<MyService>(application, "00");
   application.attach(service);

   service->setPredecessor("undefined");

   BOOST_REQUIRE_THROW(application.start(), adt::RuntimeException);;

   BOOST_REQUIRE_EQUAL(service->getInitializedCounter(), 0);
   BOOST_REQUIRE_EQUAL(service->getStoppedCounter(), 0);

   BOOST_REQUIRE_EQUAL(application.isStopped(), true);
   BOOST_REQUIRE_EQUAL(application.isRunning(), false);
}

BOOST_AUTO_TEST_CASE( repeat_predecessor )
{
   SmallestApplication application;

   std::shared_ptr<MyService> service = std::make_shared<MyService>(application, "00");
   service->setPredecessor("SomeRepeatedName");
   service->setPredecessor("SomeRepeatedName");

   BOOST_REQUIRE_NO_THROW(application.start());
}

BOOST_AUTO_TEST_CASE( interdependence_predecessor )
{
   SmallestApplication application;

   std::shared_ptr<MyService> service00 = std::make_shared<MyService>(application, "00");
   application.attach(service00);

   std::shared_ptr<MyService> service01 = std::make_shared<MyService>(application, "01");
   application.attach(service01);

   service00->setPredecessor("01");
   service01->setPredecessor("00");

   BOOST_REQUIRE_THROW(application.start(), adt::RuntimeException);;

   BOOST_REQUIRE_EQUAL(service00->getInitializedCounter(), 0);
   BOOST_REQUIRE_EQUAL(service00->getStoppedCounter(), 0);

   BOOST_REQUIRE_EQUAL(service01->getInitializedCounter(), 0);
   BOOST_REQUIRE_EQUAL(service01->getStoppedCounter(), 0);

   BOOST_REQUIRE_EQUAL(application.isStopped(), true);
   BOOST_REQUIRE_EQUAL(application.isRunning(), false);
}

BOOST_AUTO_TEST_CASE( iterator_service )
{
   SmallestApplication application;

   application.attach(std::make_shared<MyService>(application, "00"));
   application.attach(std::make_shared<MyService>(application, "01"));
   application.attach(std::make_shared<MyService>(application, "02"));
   application.attach(std::make_shared<MyService>(application, "03"));
   application.attach(std::make_shared<MyService>(application, "04"));

   BOOST_REQUIRE_NO_THROW(application.start());

   const SmallestApplication& constApplication(application);

   auto ii = application.service_begin();
   auto maxii = application.service_end();

   app::Application::const_service_iterator jj = constApplication.service_begin();
   app::Application::const_service_iterator  maxjj =  constApplication.service_end();

   while (ii != maxii) {
	   BOOST_REQUIRE_EQUAL (app::Application::service(ii)->asString(), app::Application::service(jj)->asString());
	   BOOST_REQUIRE(jj != maxjj);
	   ++ ii;
	   ++ jj;
   }
}

BOOST_AUTO_TEST_CASE( app_already_run )
{
   app::ApplicationServiceStarter application("app_already_run");

   auto thread = std::thread(parallelRun, std::ref(application));

   application.waitUntilRunning();
   BOOST_CHECK_THROW(application.start(), adt::RuntimeException);
   application.requestStop();
   thread.join();
}

BOOST_AUTO_TEST_CASE( app_stop_services )
{
   app::ApplicationServiceStarter application("app_stop_services");
   auto service = std::make_shared<MyService>(application, "00");
   application.attach(service);

   auto thread = std::thread(parallelRun, std::ref(application));
   application.waitUntilRunning();
   BOOST_CHECK(application.isRunning());

   BOOST_CHECK(service->isRunning());
   BOOST_CHECK_EQUAL(service->getInitializedCounter(), 1);
   BOOST_CHECK_EQUAL(service->getStoppedCounter(), 0);

   application.requestStop();
   thread.join();

   BOOST_CHECK(service->isStopped());
   BOOST_CHECK_EQUAL(service->getInitializedCounter(), 1);
   BOOST_CHECK_EQUAL(service->getStoppedCounter(), 1);

}

BOOST_AUTO_TEST_CASE( app_null_service )
{
   std::shared_ptr<MyService> service;
   SmallestApplication application;
   BOOST_REQUIRE_THROW(application.attach(service), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE( app_already_defined )
{
   SmallestApplication application;
   application.attach(std::make_shared<MyService>(application, "00"));
   auto iiprev = application.service_begin();
   auto iimaxprev = application.service_end();

   application.attach(std::make_shared<MyService>(application, "00"));
   BOOST_REQUIRE(iiprev == application.service_begin());
   BOOST_REQUIRE(iimaxprev == application.service_end());
}

struct ApplicationFixture {
   ApplicationFixture() : application("ApplicationFixture"),
      externalStop(false)
   {
      service = std::make_shared<MyService>(application, "my-service");
      application.attach(service);
      thr = std::thread(parallelRun, std::ref(application));

      application.waitUntilRunning();
      BOOST_CHECK(application.isRunning());
      BOOST_CHECK(service->isRunning());
   }
   virtual ~ApplicationFixture() {
      if (!externalStop) {
         try {
            application.requestStop();
         }
         catch(adt::Exception& ex) {

         }
         thr.join();
      }

      BOOST_CHECK(application.isStopped());
      BOOST_CHECK(service->isStopped());

   }
   app::ApplicationServiceStarter application;
   std::shared_ptr<app::Service> service;
   std::thread thr;
   bool externalStop;
};

BOOST_FIXTURE_TEST_CASE( app_unstoppable_stop_services, ApplicationFixture)
{
   auto service = std::make_shared<NoStopService>(application, "00");
   application.attach(service);
   application.attach(std::make_shared<NoStopService>(application, "01"));
   application.attach(std::make_shared<NoStopService>(application, "02"));

   BOOST_CHECK_THROW(application.requestStop(), adt::RuntimeException);
   thr.join();
   externalStop = true;
}

class NoAcceptRequestStopService : public app::Service {
public:
   NoAcceptRequestStopService(app::Application& application, const char* name) : app::Service(application, name) {;}

private:
   void do_initialize() throw(adt::RuntimeException);
   void do_requestStop() throw(adt::RuntimeException);
   void do_stop() throw(adt::RuntimeException);
};

void NoAcceptRequestStopService::do_initialize() throw(adt::RuntimeException) {;}

void NoAcceptRequestStopService::do_requestStop()
   throw(adt::RuntimeException)
{
   COFFEE_THROW_EXCEPTION("I dont accept request stop");
}

void NoAcceptRequestStopService::do_stop() throw(adt::RuntimeException) { ; }

BOOST_FIXTURE_TEST_CASE( app_unstoppable_requeststop_services, ApplicationFixture)
{
   auto service = std::make_shared<NoAcceptRequestStopService>(application, "00");
   application.attach(service);
   application.attach(std::make_shared<NoAcceptRequestStopService>(application, "01"));
   application.attach(std::make_shared<NoAcceptRequestStopService>(application, "02"));

   BOOST_CHECK_THROW(application.requestStop(), adt::RuntimeException);
   thr.join();
   externalStop = true;
}

BOOST_FIXTURE_TEST_CASE( service_attach_after_running, ApplicationFixture)
{
   std::shared_ptr<MyService> secondService = std::make_shared<MyService>(application, "01");
   BOOST_CHECK(!secondService->isRunning());

   application.attach(secondService);
   BOOST_CHECK(secondService->isRunning());
}

BOOST_FIXTURE_TEST_CASE( app_logger_level_change_onair, ApplicationFixture)
{
   logger::Logger::setLevel(logger::Level::Information);

   std::raise(SIGUSR2);
   BOOST_CHECK_EQUAL(logger::Logger::getLevel(), logger::Level::Debug);

   std::raise(SIGUSR2);
   BOOST_CHECK_EQUAL(logger::Logger::getLevel(), logger::Level::Error);

   std::raise(SIGUSR2);
   BOOST_CHECK_EQUAL(logger::Logger::getLevel(), logger::Level::Warning);
}

struct RemoveContextFilenameFixture : ApplicationFixture {
   RemoveContextFilenameFixture() {
      remove(application.getOutputContextFilename().c_str());

   }
   ~RemoveContextFilenameFixture() {
      remove(application.getOutputContextFilename().c_str());
   }
};

BOOST_FIXTURE_TEST_CASE( app_write_context, RemoveContextFilenameFixture )
{
   application.attach(std::make_shared<MyService>(application, "00"));
   application.attach(std::make_shared<MyService>(application, "01"));

   std::raise(SIGUSR1);

   xml::Document document;

   BOOST_CHECK_NO_THROW(document.parse(application.getOutputContextFilename()));

   auto root = document.getRoot();

   auto app = root->lookupChild("app.Application");
   auto services = app->lookupChild("Services");

   BOOST_CHECK_EQUAL(services->childAt(0)->lookupChild("Runnable")->lookupAttribute("Name")->getValue(), "my-service");
   BOOST_CHECK_EQUAL(services->childAt(1)->lookupChild("Runnable")->lookupAttribute("Name")->getValue(), "00");
   BOOST_CHECK_EQUAL(services->childAt(2)->lookupChild("Runnable")->lookupAttribute("Name")->getValue(), "01");
}

BOOST_FIXTURE_TEST_CASE( app_cannot_write_context, RemoveContextFilenameFixture )
{
   application.setOutputContextFilename("/root/file-without-access.context");

   std::raise(SIGUSR1);

   BOOST_REQUIRE_THROW(boost::filesystem::exists(application.getOutputContextFilename()), std::runtime_error);
}

