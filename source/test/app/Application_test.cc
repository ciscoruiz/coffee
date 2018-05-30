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

#include <gtest/gtest.h>

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

TEST(ApplicationTest, smallest )
{ 
   TestApplication application;

   ASSERT_THROW(application.select(app::Feature::UserDefined, app::Service::WhateverImplementation), basis::RuntimeException);

   std::shared_ptr<MyService> service = std::make_shared<MyService>(application, "TheMagical");
   application.attach(service);
   application.start();

   ASSERT_EQ(getpid(), application.getPid());
   ASSERT_EQ(0, application.getVersion().find("1.0/"));

   ASSERT_TRUE(application.select(app::Feature::UserDefined, app::Service::WhateverImplementation) == service);
   ASSERT_EQ(1, service->getInitializedCounter());
   ASSERT_EQ(1, service->getStoppedCounter());
}

TEST(ApplicationTest, select )
{
   TestApplication application;

   std::shared_ptr<MyService> service = std::make_shared<MyService>(application, "TheMagical", app::Feature::Database);
   application.attach(service);
   application.start();

   ASSERT_TRUE(application.select(app::Feature::Database, app::Service::WhateverImplementation) == service);
   ASSERT_TRUE(application.select(app::Feature::Database, "TheMagical") == service);
   ASSERT_THROW(application.select(app::Feature::Database, "Nonexisting"), basis::RuntimeException);
}

TEST(ApplicationTest, status )
{
   TestApplication application;

   ASSERT_TRUE(application.isStopped());
   ASSERT_FALSE(application.isRunning());

   application.start();

   ASSERT_TRUE(application.isStopped());
   ASSERT_FALSE(application.isRunning());
}

TEST(ApplicationTest, undefined_requirement )
{
   TestApplication application;

   std::shared_ptr<MyService> service = std::make_shared<MyService>(application, "00");
   application.attach(service);

   ASSERT_NO_THROW(service->require(app::Feature::Database));

   ASSERT_THROW(application.start(), basis::RuntimeException);

   ASSERT_EQ(0, service->getInitializedCounter());
   ASSERT_EQ(0, service->getStoppedCounter());

   ASSERT_TRUE(application.isStopped());
   ASSERT_FALSE(application.isRunning());
}

TEST(ApplicationTest, repeat_requirement )
{
   TestApplication application;

   std::shared_ptr<MyService> service = std::make_shared<MyService>(application, "00");

   ASSERT_NO_THROW(service->require(app::Feature::Database));
   ASSERT_THROW(service->require(app::Feature::Database), basis::RuntimeException);

   ASSERT_NO_THROW(application.start());
}

TEST(ApplicationTest, interdependence_requirement )
{
   TestApplication application;

   std::shared_ptr<MyService> service00 = std::make_shared<MyService>(application, "00", app::Feature::Database);
   application.attach(service00);

   std::shared_ptr<MyService> service01 = std::make_shared<MyService>(application, "01", app::Feature::UserDefined);
   application.attach(service01);

   service00->require(app::Feature::UserDefined);
   service01->require(app::Feature::Database);

   ASSERT_THROW(application.start(), basis::RuntimeException);;

   ASSERT_EQ(0, service00->getInitializedCounter());
   ASSERT_EQ(0, service00->getStoppedCounter());

   ASSERT_EQ(0, service01->getInitializedCounter());
   ASSERT_EQ(0, service01->getStoppedCounter());

   ASSERT_TRUE(application.isStopped());
   ASSERT_FALSE(application.isRunning());
}

TEST(ApplicationTest, app_server_getrequirement )
{
   TestApplication application;

   std::shared_ptr<MyService> service00 = std::make_shared<MyService>(application, "00", app::Feature::Database);
   application.attach(service00);

   std::shared_ptr<MyService> service01 = std::make_shared<MyService>(application, "01", app::Feature::UserDefined);
   application.attach(service01);

   service00->require(app::Feature::UserDefined);

   ASSERT_NO_THROW(application.start());

   ASSERT_TRUE(service00->getRequirement(app::Feature::UserDefined) == service01);
}

TEST(ApplicationTest, app_server_unknow_requirement )
{
   TestApplication application;

   std::shared_ptr<MyService> service00 = std::make_shared<MyService>(application, "00", app::Feature::Database);
   application.attach(service00);

   ASSERT_NO_THROW(application.start());
   ASSERT_THROW(service00->getRequirement(app::Feature::UserDefined), basis::RuntimeException);
}

TEST(ApplicationTest, app_server_requirement_before_start )
{
   TestApplication application;

   std::shared_ptr<MyService> service00 = std::make_shared<MyService>(application, "00", app::Feature::Database);
   application.attach(service00);

   std::shared_ptr<MyService> service01 = std::make_shared<MyService>(application, "01", app::Feature::UserDefined);
   application.attach(service01);

   service00->require(app::Feature::UserDefined);

   ASSERT_THROW(service00->getRequirement(app::Feature::UserDefined), basis::RuntimeException);
}

TEST(ApplicationTest, app_server_asstring ) {
   TestApplication application;

   std::shared_ptr<MyService> service00 = std::make_shared<MyService>(application, "00", app::Feature::Database);
   application.attach(service00);

   ASSERT_EQ("app.Service { app.Runnable { Name=Database:00 | Status= Stopped }, Requirements=[] }", service00->asString());

   ASSERT_NO_THROW(service00->require(app::Feature::UserDefined));
   ASSERT_EQ("app.Service { app.Runnable { Name=Database:00 | Status= Stopped }, Requirements=[{ Feature=UserDefined }] }", service00->asString());

   ASSERT_NO_THROW(service00->require(app::Feature::Networking, "native-sockets"));
   ASSERT_EQ("app.Service { app.Runnable { Name=Database:00 | Status= Stopped }, Requirements=[{ Feature=UserDefined },{ Feature=Networking,Implementation=native-sockets }] }", service00->asString());
}

TEST(ApplicationTest, app_server_asstring_selected ) {
   TestApplication application;

   std::shared_ptr<MyService> service00 = std::make_shared<MyService>(application, "00", app::Feature::Database);
   application.attach(service00);
   application.attach(std::make_shared<MyService>(application, "01", app::Feature::UserDefined));

   ASSERT_NO_THROW(service00->require(app::Feature::UserDefined, "01"));
   ASSERT_NO_THROW(application.start());

   ASSERT_EQ("app.Service { app.Runnable { Name=Database:00 | Status= Stopped }, Requirements=[{ Feature=UserDefined,Implementation=01,Selection=UserDefined:01 }] }", service00->asString());
}

TEST(ApplicationTest, requirement_already_prepared ) {
   TestApplication application;

   application.attach(std::make_shared<MyService>(application, "00", app::Feature::Database));

   std::shared_ptr<MyService> service01 = std::make_shared<MyService>(application, "01", app::Feature::UserDefined);
   application.attach(service01);

   ASSERT_NO_THROW(service01->require(app::Feature::Database));
   ASSERT_NO_THROW(application.start());
}

TEST(ApplicationTest, app_already_run )
{
   // Without "test" to cover the case in the coverage measurement
   app::ApplicationServiceStarter application("app_already_run");

   auto thread = std::thread(parallelRun, std::ref(application));

   application.waitUntilRunning();
   ASSERT_THROW(application.start(), basis::RuntimeException);
   EXPECT_TRUE(application.stop());
   thread.join();
   EXPECT_TRUE(!application.stop());
}

TEST(ApplicationTest, app_stop_services )
{
   app::ApplicationServiceStarter application("test_app_stop_services");
   auto service = std::make_shared<MyService>(application, "00");
   application.attach(service);

   auto thread = std::thread(parallelRun, std::ref(application));
   application.waitUntilRunning();
   EXPECT_TRUE(application.isRunning());

   EXPECT_TRUE(service->isRunning());
   EXPECT_EQ(1, service->getInitializedCounter());
   EXPECT_EQ(0, service->getStoppedCounter());

   application.stop();
   thread.join();

   EXPECT_TRUE(service->isStopped());
   EXPECT_EQ(1, service->getInitializedCounter());
   EXPECT_EQ(1, service->getStoppedCounter());
}

TEST(ApplicationTest, app_null_service )
{
   std::shared_ptr<MyService> service;
   TestApplication application;
   ASSERT_THROW(application.attach(service), basis::RuntimeException);
}

TEST(ApplicationTest, app_already_defined )
{
   TestApplication application;
   ASSERT_TRUE(application.attach(std::make_shared<MyService>(application, "00")));
   ASSERT_TRUE(!application.attach(std::make_shared<MyService>(application, "00")));

   ASSERT_TRUE(application.attach(std::make_shared<MyService>(application, "01")));
   ASSERT_TRUE(application.attach(std::make_shared<MyService>(application, "01", app::Feature::Database)));
   ASSERT_TRUE(application.attach(std::make_shared<MyService>(application, "02", app::Feature::Database)));
}

struct ApplicationRunningTest : public ::testing::Test {
   ApplicationRunningTest() : application("TestApplicationRunningTest"),
      externalStop(false)
   {
      service = std::make_shared<MyService>(application, "my-service");
      application.attach(service);
      thr = std::thread(parallelRun, std::ref(application));

      application.waitUntilRunning();
      EXPECT_TRUE(application.isRunning());
      EXPECT_TRUE(service->isRunning());
   }
   virtual ~ApplicationRunningTest() {
      if (!externalStop) {
         try {
            application.stop();
         }
         catch(basis::Exception& ex) {

         }
         thr.join();
      }

      EXPECT_TRUE(application.isStopped());
      EXPECT_TRUE(service->isStopped());

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

TEST_F(ApplicationRunningTest, unstoppable_stop_services)
{
   auto service = std::make_shared<NoStopService>(application, "00");
   application.attach(service);
   application.attach(std::make_shared<NoStopService>(application, "01"));
   application.attach(std::make_shared<NoStopService>(application, "02"));

   EXPECT_THROW(application.stop(), basis::RuntimeException);
   thr.join();
   externalStop = true;

   EXPECT_TRUE(service->isStoppedWithError() == true);
}

TEST_F(ApplicationRunningTest, service_attach_after_running)
{
   std::shared_ptr<MyService> secondService = std::make_shared<MyService>(application, "01");
   EXPECT_TRUE(!secondService->isRunning());

   application.attach(secondService);
   EXPECT_TRUE(secondService->isRunning());
}

TEST_F(ApplicationRunningTest, logger_level_change_onair)
{
   logger::Logger::setLevel(logger::Level::Information);

   std::raise(SIGUSR2);
   EXPECT_EQ(logger::Level::Debug, logger::Logger::getLevel());

   std::raise(SIGUSR2);
   EXPECT_EQ(logger::Level::Error, logger::Logger::getLevel());

   std::raise(SIGUSR2);
   EXPECT_EQ(logger::Level::Warning, logger::Logger::getLevel());
}

struct ApplicationContextTest : ApplicationRunningTest {
   ApplicationContextTest() {
      remove(application.getOutputContextFilename().c_str());

   }
   ~ApplicationContextTest() {
      remove(application.getOutputContextFilename().c_str());
   }
};

TEST_F(ApplicationContextTest, write_context)
{
   application.attach(std::make_shared<MyService>(application, "00", app::Feature::Networking));
   application.attach(std::make_shared<MyService>(application, "01"));

   std::raise(SIGUSR1);

   xml::Document document;

   EXPECT_NO_THROW(document.parse(application.getOutputContextFilename()));

   auto root = document.getRoot();

   auto app = root->lookupChild("app.Application");
   auto services = app->lookupChild("Services");

   EXPECT_EQ("Networking:00", services->childAt(0)->lookupChild("Runnable")->lookupAttribute("Name")->getValue());
   EXPECT_EQ("UserDefined:my-service", services->childAt(1)->lookupChild("Runnable")->lookupAttribute("Name")->getValue());
   EXPECT_EQ("UserDefined:01", services->childAt(2)->lookupChild("Runnable")->lookupAttribute("Name")->getValue());
}

TEST_F(ApplicationContextTest, cannot_write_context)
{
   application.setOutputContextFilename("/root/file-without-access.context");

   std::raise(SIGUSR1);

   ASSERT_THROW(boost::filesystem::exists(application.getOutputContextFilename()), std::runtime_error);
}

