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

#include <condition_variable>
#include <csignal>
#include <iostream>
#include <limits.h>
#include <mutex>
#include <signal.h>
#include <thread>

#include <boost/test/unit_test.hpp>

#include <coffee/app/ApplicationServiceStarter.hpp>
#include <coffee/app/Service.hpp>
#include <coffee/basis/Semaphore.hpp>
#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TtyWriter.hpp>
#include <coffee/xml/Document.hpp>
#include <coffee/xml/Node.hpp>

using namespace std;
using namespace coffee;

class TestApplication : public app::Application {
public:
   TestApplication() : app::Application("TestApplication", "This is the title", "1.0") {}

   void run() throw(basis::RuntimeException) {;}
};

void parallelRun(app::Application& app) {
   app.start();
}

class MyService : public app::Service {
public:
   MyService(app::Application& application, const char* implementation, const app::Feature::_v feature = app::Feature::UserDefined) :
      Service(application, feature, implementation), m_initilized(0), m_stopped(0)
   {;}
   int getInitializedCounter() const noexcept { return m_initilized; }
   int getStoppedCounter() const noexcept { return m_stopped; }

private:
   int m_initilized;
   int m_stopped;

   void do_initialize() throw(basis::RuntimeException);
   void do_stop() throw(basis::RuntimeException);
};

void MyService::do_initialize() throw(basis::RuntimeException){ ++ m_initilized;}
void MyService::do_stop() throw(basis::RuntimeException) { ++ m_stopped;}

BOOST_AUTO_TEST_CASE( smallest_application )
{ 
   TestApplication application;

   BOOST_REQUIRE_THROW(application.select(app::Feature::UserDefined, app::Service::WhateverImplementation), basis::RuntimeException);

   std::shared_ptr<MyService> service = std::make_shared<MyService>(application, "TheMagical");
   application.attach(service);
   application.start();

   BOOST_REQUIRE_EQUAL(application.getPid(), getpid());
   BOOST_REQUIRE_EQUAL(application.getVersion().find("1.0/"), 0);

   BOOST_REQUIRE(application.select(app::Feature::UserDefined, app::Service::WhateverImplementation) == service);
   BOOST_REQUIRE_EQUAL(service->getInitializedCounter(), 1);
   BOOST_REQUIRE_EQUAL(service->getStoppedCounter(), 1);
}

BOOST_AUTO_TEST_CASE( application_select )
{
   TestApplication application;

   std::shared_ptr<MyService> service = std::make_shared<MyService>(application, "TheMagical", app::Feature::Database);
   application.attach(service);
   application.start();

   BOOST_REQUIRE(application.select(app::Feature::Database, app::Service::WhateverImplementation) == service);
   BOOST_REQUIRE(application.select(app::Feature::Database, "TheMagical") == service);
   BOOST_REQUIRE_THROW(application.select(app::Feature::Database, "Nonexisting"), basis::RuntimeException);
}

BOOST_AUTO_TEST_CASE( status_application )
{
   TestApplication application;

   BOOST_REQUIRE_EQUAL(application.isStopped(), true);
   BOOST_REQUIRE_EQUAL(application.isRunning(), false);

   application.start();

   BOOST_REQUIRE_EQUAL(application.isStopped(), true);
   BOOST_REQUIRE_EQUAL(application.isRunning(), false);
}

BOOST_AUTO_TEST_CASE( undefined_requirement )
{
   TestApplication application;

   std::shared_ptr<MyService> service = std::make_shared<MyService>(application, "00");
   application.attach(service);

   BOOST_REQUIRE_NO_THROW(service->require(app::Feature::Database));

   BOOST_REQUIRE_THROW(application.start(), basis::RuntimeException);

   BOOST_REQUIRE_EQUAL(service->getInitializedCounter(), 0);
   BOOST_REQUIRE_EQUAL(service->getStoppedCounter(), 0);

   BOOST_REQUIRE_EQUAL(application.isStopped(), true);
   BOOST_REQUIRE_EQUAL(application.isRunning(), false);
}

BOOST_AUTO_TEST_CASE( repeat_requirement )
{
   TestApplication application;

   std::shared_ptr<MyService> service = std::make_shared<MyService>(application, "00");

   BOOST_REQUIRE_NO_THROW(service->require(app::Feature::Database));
   BOOST_REQUIRE_THROW(service->require(app::Feature::Database), basis::RuntimeException);

   BOOST_REQUIRE_NO_THROW(application.start());
}

BOOST_AUTO_TEST_CASE( interdependence_requirement )
{
   TestApplication application;

   std::shared_ptr<MyService> service00 = std::make_shared<MyService>(application, "00", app::Feature::Database);
   application.attach(service00);

   std::shared_ptr<MyService> service01 = std::make_shared<MyService>(application, "01", app::Feature::UserDefined);
   application.attach(service01);

   service00->require(app::Feature::UserDefined);
   service01->require(app::Feature::Database);

   BOOST_REQUIRE_THROW(application.start(), basis::RuntimeException);;

   BOOST_REQUIRE_EQUAL(service00->getInitializedCounter(), 0);
   BOOST_REQUIRE_EQUAL(service00->getStoppedCounter(), 0);

   BOOST_REQUIRE_EQUAL(service01->getInitializedCounter(), 0);
   BOOST_REQUIRE_EQUAL(service01->getStoppedCounter(), 0);

   BOOST_REQUIRE_EQUAL(application.isStopped(), true);
   BOOST_REQUIRE_EQUAL(application.isRunning(), false);
}

BOOST_AUTO_TEST_CASE( app_server_getrequirement )
{
   TestApplication application;

   std::shared_ptr<MyService> service00 = std::make_shared<MyService>(application, "00", app::Feature::Database);
   application.attach(service00);

   std::shared_ptr<MyService> service01 = std::make_shared<MyService>(application, "01", app::Feature::UserDefined);
   application.attach(service01);

   service00->require(app::Feature::UserDefined);

   BOOST_REQUIRE_NO_THROW(application.start());

   BOOST_REQUIRE(service00->getRequirement(app::Feature::UserDefined) == service01);
}

BOOST_AUTO_TEST_CASE( app_server_unknow_requirement )
{
   TestApplication application;

   std::shared_ptr<MyService> service00 = std::make_shared<MyService>(application, "00", app::Feature::Database);
   application.attach(service00);

   BOOST_REQUIRE_NO_THROW(application.start());
   BOOST_REQUIRE_THROW(service00->getRequirement(app::Feature::UserDefined), basis::RuntimeException);
}

BOOST_AUTO_TEST_CASE( app_server_requirement_before_start )
{
   TestApplication application;

   std::shared_ptr<MyService> service00 = std::make_shared<MyService>(application, "00", app::Feature::Database);
   application.attach(service00);

   std::shared_ptr<MyService> service01 = std::make_shared<MyService>(application, "01", app::Feature::UserDefined);
   application.attach(service01);

   service00->require(app::Feature::UserDefined);

   BOOST_REQUIRE_THROW(service00->getRequirement(app::Feature::UserDefined), basis::RuntimeException);
}

BOOST_AUTO_TEST_CASE( app_server_asstring ) {
   TestApplication application;

   std::shared_ptr<MyService> service00 = std::make_shared<MyService>(application, "00", app::Feature::Database);
   application.attach(service00);

   BOOST_REQUIRE_EQUAL(service00->asString(), "app.Service { app.Runnable { Name=Database:00 | Status= Stopped }, Requirements=[] }");

   BOOST_REQUIRE_NO_THROW(service00->require(app::Feature::UserDefined));
   BOOST_REQUIRE_EQUAL(service00->asString(), "app.Service { app.Runnable { Name=Database:00 | Status= Stopped }, Requirements=[{ Feature=UserDefined }] }");

   BOOST_REQUIRE_NO_THROW(service00->require(app::Feature::Networking, "native-sockets"));
   BOOST_REQUIRE_EQUAL(service00->asString(), "app.Service { app.Runnable { Name=Database:00 | Status= Stopped }, Requirements=[{ Feature=UserDefined },{ Feature=Networking,Implementation=native-sockets }] }");
}

BOOST_AUTO_TEST_CASE( app_server_asstring_selected ) {
   TestApplication application;

   std::shared_ptr<MyService> service00 = std::make_shared<MyService>(application, "00", app::Feature::Database);
   application.attach(service00);
   application.attach(std::make_shared<MyService>(application, "01", app::Feature::UserDefined));

   BOOST_REQUIRE_NO_THROW(service00->require(app::Feature::UserDefined, "01"));
   BOOST_REQUIRE_NO_THROW(application.start());

   BOOST_REQUIRE_EQUAL(service00->asString(), "app.Service { app.Runnable { Name=Database:00 | Status= Stopped }, Requirements=[{ Feature=UserDefined,Implementation=01,Selection=UserDefined:01 }] }");

}

BOOST_AUTO_TEST_CASE( app_requirement_already_prepared ) {
   TestApplication application;

   application.attach(std::make_shared<MyService>(application, "00", app::Feature::Database));

   std::shared_ptr<MyService> service01 = std::make_shared<MyService>(application, "01", app::Feature::UserDefined);
   application.attach(service01);

   BOOST_REQUIRE_NO_THROW(service01->require(app::Feature::Database));
   BOOST_REQUIRE_NO_THROW(application.start());
}

BOOST_AUTO_TEST_CASE( app_already_run )
{
   // Without "test" to cover the case in the coverage measurement
   app::ApplicationServiceStarter application("app_already_run");

   auto thread = std::thread(parallelRun, std::ref(application));

   application.waitUntilRunning();
   BOOST_CHECK_THROW(application.start(), basis::RuntimeException);
   BOOST_CHECK(application.stop());
   thread.join();
   BOOST_CHECK(!application.stop());
}

BOOST_AUTO_TEST_CASE( app_stop_services )
{
   app::ApplicationServiceStarter application("test_app_stop_services");
   auto service = std::make_shared<MyService>(application, "00");
   application.attach(service);

   auto thread = std::thread(parallelRun, std::ref(application));
   application.waitUntilRunning();
   BOOST_CHECK(application.isRunning());

   BOOST_CHECK(service->isRunning());
   BOOST_CHECK_EQUAL(service->getInitializedCounter(), 1);
   BOOST_CHECK_EQUAL(service->getStoppedCounter(), 0);

   application.stop();
   thread.join();

   BOOST_CHECK(service->isStopped());
   BOOST_CHECK_EQUAL(service->getInitializedCounter(), 1);
   BOOST_CHECK_EQUAL(service->getStoppedCounter(), 1);
}

BOOST_AUTO_TEST_CASE( app_null_service )
{
   std::shared_ptr<MyService> service;
   TestApplication application;
   BOOST_REQUIRE_THROW(application.attach(service), basis::RuntimeException);
}

BOOST_AUTO_TEST_CASE( app_already_defined )
{
   TestApplication application;
   BOOST_REQUIRE(application.attach(std::make_shared<MyService>(application, "00")));
   BOOST_REQUIRE(!application.attach(std::make_shared<MyService>(application, "00")));

   BOOST_REQUIRE(application.attach(std::make_shared<MyService>(application, "01")));
   BOOST_REQUIRE(application.attach(std::make_shared<MyService>(application, "01", app::Feature::Database)));
   BOOST_REQUIRE(application.attach(std::make_shared<MyService>(application, "02", app::Feature::Database)));
}

struct ApplicationFixture {
   ApplicationFixture() : application("TestApplicationFixture"),
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
            application.stop();
         }
         catch(basis::Exception& ex) {

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

class NoStopService : public app::Service {
public:
   NoStopService(app::Application& application, const char* name) : app::Service(application, app::Feature::UserDefined, name) {;}

private:
   void do_initialize() throw(basis::RuntimeException);
   void do_stop() throw(basis::RuntimeException);
};

void NoStopService::do_initialize() throw(basis::RuntimeException) {;}

void NoStopService::do_stop()
   throw(basis::RuntimeException)
{
   COFFEE_THROW_EXCEPTION("I dont want to stop");
}

BOOST_FIXTURE_TEST_CASE( app_unstoppable_stop_services, ApplicationFixture)
{
   auto service = std::make_shared<NoStopService>(application, "00");
   application.attach(service);
   application.attach(std::make_shared<NoStopService>(application, "01"));
   application.attach(std::make_shared<NoStopService>(application, "02"));

   BOOST_CHECK_THROW(application.stop(), basis::RuntimeException);
   thr.join();
   externalStop = true;

   BOOST_CHECK(service->isStoppedWithError() == true);
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
   application.attach(std::make_shared<MyService>(application, "00", app::Feature::Networking));
   application.attach(std::make_shared<MyService>(application, "01"));

   std::raise(SIGUSR1);

   xml::Document document;

   BOOST_CHECK_NO_THROW(document.parse(application.getOutputContextFilename()));

   auto root = document.getRoot();

   auto app = root->lookupChild("app.Application");
   auto services = app->lookupChild("Services");

   BOOST_CHECK_EQUAL(services->childAt(0)->lookupChild("Runnable")->lookupAttribute("Name")->getValue(), "Networking:00");
   BOOST_CHECK_EQUAL(services->childAt(1)->lookupChild("Runnable")->lookupAttribute("Name")->getValue(), "UserDefined:my-service");
   BOOST_CHECK_EQUAL(services->childAt(2)->lookupChild("Runnable")->lookupAttribute("Name")->getValue(), "UserDefined:01");
}

BOOST_FIXTURE_TEST_CASE( app_cannot_write_context, RemoveContextFilenameFixture )
{
   application.setOutputContextFilename("/root/file-without-access.context");

   std::raise(SIGUSR1);

   BOOST_REQUIRE_THROW(boost::filesystem::exists(application.getOutputContextFilename()), std::runtime_error);
}

