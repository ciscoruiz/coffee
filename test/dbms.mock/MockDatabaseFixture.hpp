/*
 * MockDatabaseFixture.hpp
 *
 *  Created on: Mar 4, 2018
 *      Author: cisco
 */

#ifndef TEST_DBMS_MOCK_MOCKFIXTURE_HPP_
#define TEST_DBMS_MOCK_MOCKFIXTURE_HPP_

#include <coffee/app/ApplicationEngineRunner.hpp>

#include <coffee/dbms/Database.hpp>
#include <coffee/dbms/Connection.hpp>

template <class _T> struct MockDatabaseFixture {
   coffee::app::ApplicationEngineRunner app;
   std::shared_ptr<_T> database;
   std::shared_ptr<coffee::dbms::Connection> connection;
   std::thread thr;

   MockDatabaseFixture(const char* appName) : app(appName) {
      database = _T::instantiate(app);
      thr = std::thread(dbmsParallelRun, std::ref(app));
      app.waitUntilRunning();
      BOOST_REQUIRE(database->isRunning());
      connection = database->createConnection("0", "0", "0");
   }

   virtual ~MockDatabaseFixture() {
       app.requestStop();
       thr.join();
    }

   static void dbmsParallelRun(coffee::app::Application& app) {
      app.start();
   }
};


#endif /* TEST_DBMS_MOCK_MOCKFIXTURE_HPP_ */
