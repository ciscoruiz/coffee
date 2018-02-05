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

#include <memory>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TtyWriter.hpp>

#include <coffee/dbms.sqlite/SqliteDatabase.hpp>
#include <coffee/dbms.sqlite/SqliteConnection.hpp>

#include <coffee/dbms/GuardConnection.hpp>
#include <coffee/dbms/GuardStatement.hpp>
#include <coffee/dbms/Connection.hpp>
#include <coffee/dbms/Statement.hpp>
#include <coffee/dbms/datatype/String.hpp>
#include <coffee/dbms/datatype/Integer.hpp>

using namespace coffee;

struct StatementAgeGreater {
   StatementAgeGreater(dbms::Database& database, std::shared_ptr<dbms::Connection>& connection) {
      const char* sql = "select Name, age from employee where age > ?";

      inputId = std::make_shared<dbms::datatype::Integer>("age");
      outputId = std::make_shared<dbms::datatype::Integer>("age");
      outputName = std::make_shared<dbms::datatype::String>("name", 20);

      statement = database.createStatement("greater_age", sql);
      statement->createBinderInput(inputId);
      statement->createBinderOutput(outputName);
      statement->createBinderOutput(outputId);
   }
   std::shared_ptr<dbms::datatype::Integer> inputId;
   std::shared_ptr<dbms::datatype::Integer> outputId;
   std::shared_ptr<dbms::datatype::String> outputName;
   std::shared_ptr<dbms::Statement> statement;
};

struct StatementCountAgeGreater {
   StatementCountAgeGreater(dbms::Database& database, std::shared_ptr<dbms::Connection>& connection) {
      const char* sql = "select count(*) from employee where age > ?";

      inputId = std::make_shared<dbms::datatype::Integer>("age");
      outputId = std::make_shared<dbms::datatype::Integer>("count");

      statement = database.createStatement("count_greater_age", sql);
      statement->createBinderInput(inputId);
      statement->createBinderOutput(outputId);
   }
   std::shared_ptr<dbms::datatype::Integer> inputId;
   std::shared_ptr<dbms::datatype::Integer> outputId;
   std::shared_ptr<dbms::Statement> statement;
};


struct SqliteFixture {
   static  boost::filesystem::path dbPath;

   SqliteFixture() : database (dbPath) {
      logger::Logger::initialize(std::make_shared<logger::TtyWriter>());
      logger::Logger::setLevel(logger::Level::Debug);

      boost::filesystem::remove(dbPath);
      connection = database.createConnection("first", "user:first", "none");
      BOOST_REQUIRE_NO_THROW(database.externalInitialize());
      BOOST_REQUIRE_EQUAL(boost::filesystem::exists(dbPath), true);
      BOOST_REQUIRE_EQUAL(connection->isAvailable(), true);

      const char* sql = " \
         BEGIN TRANSACTION; \
         CREATE TABLE employee (Name varchar(20),Dept varchar(20),jobTitle varchar(20), age integer); \
         INSERT INTO employee VALUES('Fred Flinstone','Quarry Worker','Rock Digger', 30); \
         INSERT INTO employee VALUES('Wilma Flinstone','Finance','Analyst', 40); \
         INSERT INTO employee VALUES('Barney Rubble','Sales','Neighbor', 50); \
         INSERT INTO employee VALUES('Betty Rubble','IT','Neighbor', 60); \
         COMMIT; "; \

      BOOST_REQUIRE_NO_THROW(std::dynamic_pointer_cast<dbms::sqlite::SqliteConnection>(connection)->execute(sql));
   }

   dbms::sqlite::SqliteDatabase database;
   std::shared_ptr<dbms::Connection> connection;

};

boost::filesystem::path SqliteFixture::dbPath("/tmp/sqlite_test.db");

BOOST_FIXTURE_TEST_CASE(sqlite_create_db, SqliteFixture)
{
   auto secondConnection = database.createConnection("second", "user:second", "none");
   BOOST_REQUIRE_EQUAL(secondConnection->isAvailable(), true);
}

// See http://www.yolinux.com/TUTORIALS/SQLite.html

BOOST_FIXTURE_TEST_CASE(sqlite_multi_select, SqliteFixture)
{
   StatementAgeGreater fullStatement(database, connection);

   dbms::GuardConnection guardConnection(connection);
   dbms::GuardStatement guardStament(guardConnection, fullStatement.statement);

   fullStatement.inputId->setValue(40);

   dbms::ResultCode resultCode = guardStament.execute();

   BOOST_REQUIRE(resultCode.successful());

   int counter = 0;
   while(guardStament.fetch()) {
      BOOST_REQUIRE_GT(fullStatement.outputId->getValue(), 40);
      counter ++;
   }

   BOOST_REQUIRE_EQUAL(counter, 2);
}

BOOST_FIXTURE_TEST_CASE(sqlite_unique_select, SqliteFixture)
{
   StatementAgeGreater fullStatement(database, connection);

   dbms::GuardConnection guardConnection(connection);
   dbms::GuardStatement guardStament(guardConnection, fullStatement.statement);

   fullStatement.inputId->setValue(50);

   dbms::ResultCode resultCode = guardStament.execute();

   BOOST_REQUIRE(resultCode.successful());

   int counter = 0;
   while(guardStament.fetch()) {
      BOOST_REQUIRE_EQUAL(fullStatement.outputId->getValue(), 60);
      BOOST_REQUIRE_EQUAL(fullStatement.outputName->getValue(), std::string("Betty Rubble"));
      counter ++;
   }

   BOOST_REQUIRE_EQUAL(counter, 1);
}

BOOST_FIXTURE_TEST_CASE(sqlite_no_select, SqliteFixture)
{
   StatementAgeGreater fullStatement(database, connection);

   dbms::GuardConnection guardConnection(connection);
   dbms::GuardStatement guardStament(guardConnection, fullStatement.statement);

   fullStatement.inputId->setValue(100);

   dbms::ResultCode resultCode = guardStament.execute();

   BOOST_REQUIRE(resultCode.notFound());
   BOOST_REQUIRE(!guardStament.fetch());
}

BOOST_FIXTURE_TEST_CASE(sqlite_rebind_select, SqliteFixture)
{
   StatementCountAgeGreater fullStatement(database, connection);

   dbms::GuardConnection guardConnection(connection);
   dbms::GuardStatement guardStament(guardConnection, fullStatement.statement);

   {
      fullStatement.inputId->setValue(10);
      dbms::ResultCode resultCode = guardStament.execute();
      BOOST_REQUIRE(resultCode.successful());
      BOOST_REQUIRE(guardStament.fetch());
      BOOST_REQUIRE_EQUAL(fullStatement.outputId->getValue(), 4);
   }

   {
      fullStatement.inputId->setValue(40);
      dbms::ResultCode resultCode = guardStament.execute();
      BOOST_REQUIRE(resultCode.successful());
      BOOST_REQUIRE(guardStament.fetch());
      BOOST_REQUIRE_EQUAL(fullStatement.outputId->getValue(), 2);
   }

   {
      fullStatement.inputId->setValue(100);
      dbms::ResultCode resultCode = guardStament.execute();
      BOOST_REQUIRE(resultCode.successful());
      BOOST_REQUIRE(guardStament.fetch());
      BOOST_REQUIRE_EQUAL(fullStatement.outputId->getValue(), 0);
   }

}

