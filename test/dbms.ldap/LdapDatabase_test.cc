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

#include <vector>

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

using namespace coffee;

using coffee::dbms::ldap::LdapStatementParameters;

static void ldapParallelRun(coffee::app::Application& app) {
   app.start();
}

struct LdapFixture  {
   LdapFixture() : app ("TestLdapApplication"){
      const char* logFileName = "test/dbms.ldap/trace.log";
      unlink (logFileName);
      logger::Logger::initialize(std::make_shared<logger::UnlimitedTraceWriter>(logFileName));
      logger::Logger::setLevel(logger::Level::Debug);

      // See https://stackoverflow.com/questions/7667197/is-there-any-free-ldap-server-with-data
      database = dbms::ldap::LdapDatabase::instantiate(app, "ldap://ldap.forumsys.com");
      thr = std::thread(ldapParallelRun, std::ref(app));
      app.waitUntilRunning();
      BOOST_REQUIRE(database->isRunning());
      dbms::ldap::LdapConnectionParameters connParameters("uid=tesla,dc=example,dc=com", "password");
      BOOST_REQUIRE_NO_THROW(connection = database->createConnection("first", connParameters));
      BOOST_REQUIRE_EQUAL(connection->isAvailable(), true);

      LdapStatementParameters stParameters(dbms::ActionOnError::Ignore, "dc=example,dc=com");
      readStatement = database->createStatement("persons", "objectClass=person", stParameters.setScope(LdapStatementParameters::Scope::SubTree));
      BOOST_REQUIRE(readStatement);
   }
   ~LdapFixture() {
      app.stop();
      thr.join();
   }

   void setupReading(const char** varNames) {
      for (int ii = 0; varNames[ii] != nullptr; ++ ii) {
         auto outputBind = std::make_shared<dbms::datatype::MultiString>(varNames[ii]);
         variables.push_back(outputBind);
         BOOST_REQUIRE_NO_THROW(readStatement->createBinderOutput(outputBind));
      }
   }

   app::ApplicationServiceStarter app;
   std::shared_ptr<dbms::ldap::LdapDatabase> database;
   std::shared_ptr<dbms::Statement> readStatement;
   std::thread thr;
   std::shared_ptr<dbms::Connection> connection;
   std::vector<std::shared_ptr<dbms::datatype::MultiString> > variables;
};

BOOST_FIXTURE_TEST_CASE(ldap_dbms_bad_cast, LdapFixture)
{
   BOOST_REQUIRE_THROW(database->createConnection("second", coffee::dbms::ConnectionParameters("otheruser", "somepassword")), basis::RuntimeException);
}


BOOST_FIXTURE_TEST_CASE(ldap_dbms_statement, LdapFixture)
{
   const char* varNames[] = { "sn", "cn", "objectClass", "telephoneNumber", nullptr };

   setupReading(varNames);

   dbms::GuardConnection guardConnection(connection);
   dbms::GuardStatement guardStament(guardConnection, readStatement);
   dbms::ResultCode resultCode = guardStament.execute();

   BOOST_REQUIRE(resultCode.successful());

   int counter = 0;
   while(guardStament.fetch()) {
      for(auto ii = variables.begin(), maxii = variables.end(); ii != maxii; ++ ii) {
         auto multiString = *ii;
         BOOST_REQUIRE(multiString->hasValue());

         if (multiString->getName() == "objectClass") {
            BOOST_REQUIRE_EQUAL(multiString->size(), 4);
         }
      }
      counter ++;
   }

   BOOST_REQUIRE_EQUAL(counter, 17);
}

BOOST_FIXTURE_TEST_CASE(ldap_dbms_statement_notfound_var, LdapFixture)
{
   const char* varNames[] = { "not-found", "cn", nullptr };

   setupReading(varNames);

   dbms::GuardConnection guardConnection(connection);
   dbms::GuardStatement guardStament(guardConnection, readStatement);
   dbms::ResultCode resultCode = guardStament.execute();

   BOOST_REQUIRE(resultCode.successful());

   int counter = 0;
   while(guardStament.fetch()) {
      for(auto ii = variables.begin(), maxii = variables.end(); ii != maxii; ++ ii) {
         auto multiString = *ii;

         if (multiString->getName() == "not-found") {
            BOOST_REQUIRE(!multiString->hasValue());
         }
         else {
            BOOST_REQUIRE(multiString->hasValue());
         }
      }
      counter ++;
   }

   BOOST_REQUIRE_EQUAL(counter, 17);
}

