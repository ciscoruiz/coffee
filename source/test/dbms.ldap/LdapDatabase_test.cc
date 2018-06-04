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

#include <vector>
#include <map>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/UnlimitedTraceWriter.hpp>

#include <coffee/app/ApplicationServiceStarter.hpp>

#include <coffee/dbms/GuardConnection.hpp>
#include <coffee/dbms/GuardStatement.hpp>
#include <coffee/dbms/datatype/MultiString.hpp>
#include <coffee/dbms/Statement.hpp>

#include <coffee/dbms.ldap/LdapDatabase.hpp>
#include <coffee/dbms.ldap/LdapConnection.hpp>
#include <coffee/dbms.ldap/LdapConnectionParameters.hpp>
#include <coffee/dbms.ldap/LdapStatementParameters.hpp>
#include <coffee/dbms.ldap/LdapStatement.hpp>
#include <coffee/dbms/datatype/String.hpp>

using namespace coffee;

using coffee::dbms::ldap::LdapStatementParameters;
using coffee::dbms::datatype::Constraint;

static void ldapParallelRun(coffee::app::Application& app) {
   app.start();
}

struct LdapTestFixture : ::testing::Test {
   LdapTestFixture() : app ("TestLdapApplication"){}

   void SetUp() {
      const char* logFileName = "source/test/dbms.ldap/trace.log";
      unlink (logFileName);
      logger::Logger::initialize(std::make_shared<logger::UnlimitedTraceWriter>(logFileName));
      logger::Logger::setLevel(logger::Level::Debug);

      // See https://stackoverflow.com/questions/7667197/is-there-any-free-ldap-server-with-data
      database = dbms::ldap::LdapDatabase::instantiate(app, "ldap://ldap.forumsys.com");
      thr = std::thread(ldapParallelRun, std::ref(app));
      app.waitUntilRunning();
      ASSERT_TRUE(database->isRunning());
      dbms::ldap::LdapConnectionParameters connParameters("uid=tesla,dc=example,dc=com", "password");
      ASSERT_NO_THROW(connection = database->createConnection("first", connParameters));
      ASSERT_TRUE(connection->isAvailable());

      LdapStatementParameters stParameters(dbms::ActionOnError::Ignore, "dc=example,dc=com");
      readStatement = database->createStatement("persons", "objectClass=person", stParameters.setScope(LdapStatementParameters::Scope::SubTree));
   }
   ~LdapTestFixture() {
      app.stop();
      thr.join();
   }

   typedef std::vector< std::pair<std::string, Constraint::_v> > Definition;

   void setupReading(const Definition& definition) {
      for (auto ii = definition.begin(), maxii = definition.end(); ii != maxii; ++ ii) {
         auto outputBind = dbms::datatype::MultiString::instantiate(ii->first.c_str(), ii->second);
         variables.push_back(outputBind);
         ASSERT_NO_THROW(readStatement->createBinderOutput(outputBind));
      }
   }

   app::ApplicationServiceStarter app;
   std::shared_ptr<dbms::ldap::LdapDatabase> database;
   std::shared_ptr<dbms::Statement> readStatement;
   std::thread thr;
   std::shared_ptr<dbms::Connection> connection;
   std::vector<std::shared_ptr<dbms::datatype::MultiString> > variables;
};

TEST_F(LdapTestFixture, dbms_bad_cast)
{
   ASSERT_THROW(database->createConnection("second", coffee::dbms::ConnectionParameters("otheruser", "somepassword")), basis::RuntimeException);
}

TEST(LdapTest, bad_name) {
   app::ApplicationServiceStarter app("TestLdapApplication-badconnection");
   std::shared_ptr<dbms::ldap::LdapDatabase> database = dbms::ldap::LdapDatabase::instantiate(app, "ldap://bad.url");
   std::thread thr = std::thread(ldapParallelRun, std::ref(app));
   app.waitUntilRunning();
   ASSERT_TRUE(database->isRunning());
   dbms::ldap::LdapConnectionParameters connParameters("uid=tesla,dc=example,dc=com", "password");
   std::shared_ptr<dbms::Connection> connection;
   ASSERT_THROW(connection = database->createConnection("first", connParameters), dbms::DatabaseException);
   app.stop();
   thr.join();
}

TEST_F(LdapTestFixture, dbms_statement)
{
   Definition definition = {
      {"sn", Constraint::CanNotBeNull}, {"cn", Constraint::CanNotBeNull}, {"objectClass", Constraint::CanNotBeNull},
      {"telephoneNumber", Constraint::CanBeNull}
   };

   setupReading(definition);

   dbms::GuardConnection guardConnection(connection);
   dbms::GuardStatement guardStament(guardConnection, readStatement);
   dbms::ResultCode resultCode = guardStament.execute();

   ASSERT_TRUE(resultCode.successful());

   std::map<std::string, int> mymap = {
      {"Isaac Newton", 4}, {"Albert Einstein", 4}, {"Nikola Tesla", 5}, {"Galileo Galilei", 4}, {"Leonhard Euler", 4},
      {"Carl Friedrich Gauss", 4}, {"Bernhard Riemann", 4}, {"Euclid", 4}, {"read-only-admin", 4}, {"Test", 3},
      {"Marie Curie", 4}, {"Alfred Nobel", 4}, {"Robert Boyle", 4}, {"Louis Pasteur", 4}, {"No Group", 4},
      {"FS Training", 4}
   };

   int counter = 0;
   while(guardStament.fetch()) {
      auto cn = coffee_datatype_downcast(dbms::datatype::MultiString, guardStament.getOutputData(1));

      ASSERT_EQ(1, cn->size());
      const std::string name = dbms::datatype::MultiString::value(cn->begin_value());

      auto ii = mymap.find(name);
      ASSERT_TRUE(ii != mymap.end());

      auto objectClass = coffee_datatype_downcast(dbms::datatype::MultiString, guardStament.getOutputData(2));
      ASSERT_EQ(ii->second, objectClass->size());

      counter ++;
   }

   ASSERT_EQ(17, counter);
}

TEST_F(LdapTestFixture, dbms_statement_notfound_var)
{
   Definition definition = {
      {"sn-not-exist", Constraint::CanBeNull}, {"cn", Constraint::CanNotBeNull}
   };

   setupReading(definition);

   dbms::GuardConnection guardConnection(connection);
   dbms::GuardStatement guardStament(guardConnection, readStatement);
   dbms::ResultCode resultCode = guardStament.execute();

   ASSERT_TRUE(resultCode.successful());

   int counter = 0;
   while(guardStament.fetch()) {
      ASSERT_TRUE(!guardStament.getOutputData(0)->hasValue());
      ASSERT_TRUE(guardStament.getOutputData(1)->hasValue());
      counter ++;
   }

   ASSERT_EQ(17, counter);
}

TEST_F(LdapTestFixture, dbms_statement_input)
{
   Definition definition = {
      {"sn", Constraint::CanNotBeNull}, {"cn", Constraint::CanNotBeNull}, {"objectClass", Constraint::CanBeNull}
   };

   setupReading(definition);

   readStatement->createBinderInput(coffee::dbms::datatype::String::instantiate("sn", 10));
   readStatement->createBinderInput(coffee::dbms::datatype::String::instantiate("cn", 10));

   dbms::GuardConnection guardConnection(connection);
   dbms::GuardStatement guardStament(guardConnection, readStatement);
   dbms::ResultCode resultCode = guardStament.execute();

   ASSERT_TRUE(resultCode.successful());

   int counter = 0;
   while(guardStament.fetch()) {
      ASSERT_TRUE(guardStament.getOutputData(0)->hasValue());
      ASSERT_TRUE(guardStament.getOutputData(1)->hasValue());
      ASSERT_TRUE(!guardStament.getOutputData(2)->hasValue());
      counter ++;
   }

   ASSERT_EQ(17, counter);
}

TEST_F(LdapTestFixture, dbms_statement_output_not_nulleable)
{
   Definition definition = {
      {"sn-not-exist", Constraint::CanNotBeNull}, {"cn", Constraint::CanNotBeNull}
   };

   setupReading(definition);

   dbms::GuardConnection guardConnection(connection);
   dbms::GuardStatement guardStament(guardConnection, readStatement);
   dbms::ResultCode resultCode = guardStament.execute();

   ASSERT_TRUE(resultCode.successful());
   ASSERT_THROW(guardStament.fetch(), basis::RuntimeException);
}

TEST_F(LdapTestFixture, dbms_statement_close_before_end)
{
   Definition definition = {
      {"sn", Constraint::CanNotBeNull}, {"cn", Constraint::CanNotBeNull}, {"objectClass", Constraint::CanBeNull}
   };

   setupReading(definition);

   dbms::GuardConnection guardConnection(connection);
   dbms::GuardStatement guardStament(guardConnection, readStatement);
   dbms::ResultCode resultCode = guardStament.execute();

   ASSERT_TRUE(resultCode.successful());

   ASSERT_TRUE(guardStament.fetch());
}

TEST_F(LdapTestFixture, dbms_statement_not_found)
{
   LdapStatementParameters stParameters(dbms::ActionOnError::Ignore, "dc=example,dc=com");
   readStatement = database->createStatement("not-found", "objectClass=zzzz", stParameters.setScope(LdapStatementParameters::Scope::SubTree));

   dbms::GuardConnection guardConnection(connection);
   dbms::GuardStatement guardStament(guardConnection, readStatement);
   dbms::ResultCode resultCode = guardStament.execute();

   ASSERT_TRUE(resultCode.notFound());

   ASSERT_TRUE(!guardStament.fetch());
}
