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
#include <coffee/dbms/datatype/Float.hpp>
#include <coffee/dbms/datatype/LongBlock.hpp>
#include <coffee/dbms/datatype/ShortBlock.hpp>
#include <coffee/dbms/datatype/Date.hpp>

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

struct StatementCountAllTypes {
   StatementCountAllTypes(dbms::Database& database, std::shared_ptr<dbms::Connection>& connection) {
      const char* sql = "select count(*) from all_types";
      counter = std::make_shared<dbms::datatype::Integer>("counter");
      statement = database.createStatement("count_all_types", sql);
      statement->createBinderOutput(counter);
   }
   std::shared_ptr<dbms::datatype::Integer> counter;
   std::shared_ptr<dbms::Statement> statement;
};

struct StatementBlob {
   StatementBlob(dbms::Database& database, std::shared_ptr<dbms::Connection>& connection) {
      const char* sql = "select id, myfloat, mydata from btest where id == ?";

      id = std::make_shared<dbms::datatype::Integer>("id");
      myfloat = std::make_shared<dbms::datatype::Float>("float");
      mydata = std::make_shared<dbms::datatype::LongBlock>("data", dbms::datatype::Constraint::CanBeNull);

      statement = database.createStatement("select_blob", sql);
      statement->createBinderInput(id);
      statement->createBinderOutput(id);
      statement->createBinderOutput(myfloat);
      statement->createBinderOutput(mydata);
   }
   std::shared_ptr<dbms::datatype::Integer> id;
   std::shared_ptr<dbms::datatype::Float> myfloat;
   std::shared_ptr<dbms::datatype::LongBlock> mydata;
   std::shared_ptr<dbms::Statement> statement;
};

struct StatementSyntaxError {
   StatementSyntaxError(dbms::Database& database, std::shared_ptr<dbms::Connection>& connection) {
      const char* sql = "select age where employee where age > ?";
      input = std::make_shared<dbms::datatype::Integer>("age");
      output = std::make_shared<dbms::datatype::Integer>("age");
      statement = database.createStatement("syntax_error", sql);
      statement->createBinderInput(input);
      statement->createBinderOutput(output);
   };
   std::shared_ptr<dbms::datatype::Integer> input;
   std::shared_ptr<dbms::datatype::Integer> output;
   std::shared_ptr<dbms::Statement> statement;
};

struct InsertAllType {
   InsertAllType(dbms::Database& database, std::shared_ptr<dbms::Connection>& connection) {
      const char* sql = "insert into all_types(id, the_float, the_date, the_blob) values (?, ?, ?, ?);";
      id = std::make_shared<dbms::datatype::Integer>("id");
      theFloat = std::make_shared<dbms::datatype::Float>("the_float", dbms::datatype::Constraint::CanBeNull);
      theDate = std::make_shared<dbms::datatype::Date>("the_date", dbms::datatype::Constraint::CanBeNull);
      theBlob = std::make_shared<dbms::datatype::ShortBlock>("the_blob", 128, dbms::datatype::Constraint::CanBeNull);
      statement = database.createStatement("insert_all_types", sql);
      statement->createBinderInput(id);
      statement->createBinderInput(theFloat);
      statement->createBinderInput(theDate);
      statement->createBinderInput(theBlob);
   }

   std::shared_ptr<dbms::datatype::Integer> id;
   std::shared_ptr<dbms::datatype::Float> theFloat;
   std::shared_ptr<dbms::datatype::Date> theDate;
   std::shared_ptr<dbms::datatype::ShortBlock> theBlob;
   std::shared_ptr<dbms::Statement> statement;
};

struct SelectAllType {
   SelectAllType(dbms::Database& database, std::shared_ptr<dbms::Connection>& connection) {
      const char* sql = "select id, the_float, the_date, the_blob from all_types order by id";
      id = std::make_shared<dbms::datatype::Integer>("id");
      theFloat = std::make_shared<dbms::datatype::Float>("the_float", dbms::datatype::Constraint::CanBeNull);
      theDate = std::make_shared<dbms::datatype::Date>("the_date", dbms::datatype::Constraint::CanBeNull);
      theBlob = std::make_shared<dbms::datatype::ShortBlock>("the_blob", 128, dbms::datatype::Constraint::CanBeNull);
      statement = database.createStatement("select_all_types", sql);
      statement->createBinderOutput(id);
      statement->createBinderOutput(theFloat);
      statement->createBinderOutput(theDate);
      statement->createBinderOutput(theBlob);
   }

   std::shared_ptr<dbms::datatype::Integer> id;
   std::shared_ptr<dbms::datatype::Float> theFloat;
   std::shared_ptr<dbms::datatype::Date> theDate;
   std::shared_ptr<dbms::datatype::ShortBlock> theBlob;
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
         create table btest(ID INTEGER, myfloat float, MyData BLOB); \
         create table all_types (id integer primary key, the_float float, the_date date, the_blob blob); \
         \
         INSERT INTO employee VALUES('Fred Flinstone','Quarry Worker','Rock Digger', 30); \
         INSERT INTO employee VALUES('Wilma Flinstone','Finance','Analyst', 40); \
         INSERT INTO employee VALUES('Barney Rubble','Sales','Neighbor', 50); \
         INSERT INTO employee VALUES('Betty Rubble','IT','Neighbor', 60); \
         \
         INSERT INTO btest(ID, myfloat) values (11, 0.11); \
         INSERT INTO btest(ID, myfloat, mydata) values (22, 0.22, '123456789-12345'); \
         COMMIT; "; \

      BOOST_REQUIRE_NO_THROW(std::dynamic_pointer_cast<dbms::sqlite::SqliteConnection>(connection)->execute(sql));
   }
   ~SqliteFixture() {
      database.externalStop();
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

BOOST_AUTO_TEST_CASE(sqlite_connection_noopen)
{
   dbms::sqlite::SqliteDatabase database("/root");
   auto connection = database.createConnection("closed", "user:first", "none");
   BOOST_REQUIRE_THROW(dbms::GuardConnection guard(connection), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(sqlite_invalid_access)
{
   dbms::sqlite::SqliteDatabase database("/root");
   auto connection = database.createConnection("first", "user:first", "none");
   BOOST_REQUIRE_THROW(database.externalInitialize(), adt::RuntimeException);
   BOOST_REQUIRE(!connection->isAvailable());
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

BOOST_FIXTURE_TEST_CASE(sqlite_insert, SqliteFixture)
{
   StatementCountAgeGreater ageCounter(database, connection);

   const char* sql = "INSERT INTO employee VALUES(?, ?, ?, ?)";

   auto name = std::make_shared<dbms::datatype::String>("name", 20);
   auto dept = std::make_shared<dbms::datatype::String>("dept", 20);
   auto job = std::make_shared<dbms::datatype::String>("jobTitle", 20);
   auto age = std::make_shared<dbms::datatype::Integer>("age");

   auto insert = database.createStatement("insert_employee", sql);
   insert->createBinderInput(name);
   insert->createBinderInput(dept);
   insert->createBinderInput(job);
   insert->createBinderInput(age);

   {
      dbms::GuardConnection guardConnection(connection);
      dbms::GuardStatement guardStament(guardConnection, insert);

      name->setValue("new-name");
      dept->setValue("new-dept");
      job->setValue("new-job");
      age->setValue(31);

      dbms::ResultCode rc = guardStament.execute();

      BOOST_REQUIRE(rc.successful());
   }

   auto secondConnection = database.createConnection("second", "user:second", "none");

   {
      dbms::GuardConnection guardConnection(secondConnection);
      dbms::GuardStatement guardStament(guardConnection, ageCounter.statement);
      ageCounter.inputId->setValue(30);
      dbms::ResultCode resultCode = guardStament.execute();
      BOOST_REQUIRE(resultCode.successful());
      BOOST_REQUIRE(guardStament.fetch());
      BOOST_REQUIRE_EQUAL(ageCounter.outputId->getValue(), 3 + 1);
   }
}

BOOST_FIXTURE_TEST_CASE(sqlite_float, SqliteFixture)
{
   StatementBlob fullStatement(database, connection);

   dbms::GuardConnection guardConnection(connection);
   dbms::GuardStatement guardStament(guardConnection, fullStatement.statement);
   fullStatement.id->setValue(11);
   dbms::ResultCode resultCode = guardStament.execute();
   BOOST_REQUIRE(resultCode.successful());
   BOOST_REQUIRE(guardStament.fetch());
   BOOST_REQUIRE_CLOSE(fullStatement.myfloat->getValue(), 0.11, 0.1);
   BOOST_REQUIRE(!fullStatement.mydata->hasValue());

   fullStatement.id->setValue(22);
   resultCode = guardStament.execute();
   BOOST_REQUIRE(resultCode.successful());
   BOOST_REQUIRE(guardStament.fetch());
   BOOST_REQUIRE_CLOSE(fullStatement.myfloat->getValue(), 0.22, 0.1);

   const adt::DataBlock& dbBlob = fullStatement.mydata->getValue();
   adt::DataBlock expected("123456789-12345", 15);
   BOOST_REQUIRE_EQUAL(dbBlob.size(), expected.size());
   BOOST_REQUIRE_EQUAL(memcmp(dbBlob.data(), expected.data(), expected.size()), 0);
}

BOOST_FIXTURE_TEST_CASE(sqlite_rebind_sentence, SqliteFixture)
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

BOOST_FIXTURE_TEST_CASE(sqlite_reuse_sentence, SqliteFixture)
{
   StatementCountAgeGreater ageCounter(database, connection);

   {
      dbms::GuardConnection guardConnection(connection);
      dbms::GuardStatement guardStament(guardConnection, ageCounter.statement);

      ageCounter.inputId->setValue(10);
      dbms::ResultCode resultCode = guardStament.execute();
      BOOST_REQUIRE(resultCode.successful());
      BOOST_REQUIRE(guardStament.fetch());
      BOOST_REQUIRE_EQUAL(ageCounter.outputId->getValue(), 4);
   }

   auto secondConnection = database.createConnection("second", "user:second", "none");

   {
      dbms::GuardConnection guardConnection(secondConnection);
      dbms::GuardStatement guardStament(guardConnection, ageCounter.statement);

      ageCounter.inputId->setValue(30);
      dbms::ResultCode resultCode = guardStament.execute();
      BOOST_REQUIRE(resultCode.successful());
      BOOST_REQUIRE(guardStament.fetch());
      BOOST_REQUIRE_EQUAL(ageCounter.outputId->getValue(), 3);
   }
}

BOOST_FIXTURE_TEST_CASE(sqlite_syntax_error, SqliteFixture)
{
   StatementSyntaxError syntaxError(database, connection);

   dbms::GuardConnection guardConnection(connection);
   dbms::GuardStatement guardStament(guardConnection, syntaxError.statement);
   BOOST_REQUIRE_THROW(guardStament.execute(), dbms::DatabaseException);
}


BOOST_FIXTURE_TEST_CASE(sqlite_rollback, SqliteFixture)
{
   InsertAllType insert(database, connection);
   StatementCountAllTypes count(database, connection);

   {
      dbms::GuardConnection guardConnection(connection);
      dbms::GuardStatement guardStament(guardConnection, insert.statement);
      insert.id->setValue(100);
      insert.theBlob->isNull();
      insert.theFloat->isNull();
      insert.theDate->setValue(adt::Second::getLocalTime());
      BOOST_REQUIRE_NO_THROW(guardStament.execute());
   }

   {
      dbms::GuardConnection guardConnection(connection);
      dbms::GuardStatement guardStament(guardConnection, count.statement);
      BOOST_REQUIRE_NO_THROW(guardStament.execute());
      BOOST_REQUIRE(guardStament.fetch());
      BOOST_REQUIRE_EQUAL(count.counter->getValue(), 1);
   }

   {
      dbms::GuardConnection guardConnection(connection);
      dbms::GuardStatement guardStament(guardConnection, insert.statement);
      insert.id->setValue(100);
      BOOST_REQUIRE_THROW(guardStament.execute(), dbms::DatabaseException);
   }
}

BOOST_FIXTURE_TEST_CASE(sqlite_insert_all_types, SqliteFixture)
{
   auto now = adt::Second::getTime();

   InsertAllType insert(database, connection);
   StatementCountAllTypes count(database, connection);
   SelectAllType select(database, connection);

   {
      dbms::GuardConnection guardConnection(connection);
      dbms::GuardStatement guardStament(guardConnection, insert.statement);

      for (int ii = 0; ii < 10; ++ ii) {
         insert.id->setValue(ii);
         insert.theFloat->setValue(10.11 * ii);
         insert.theDate->setValue(now + ii);
         insert.theBlob->setValue(adt::DataBlock((const char*) &now, sizeof(now)));
         BOOST_REQUIRE_NO_THROW(guardStament.execute());
      }
   }

   {
      dbms::GuardConnection guardConnection(connection);
      dbms::GuardStatement guardStament(guardConnection, count.statement);
      BOOST_REQUIRE_NO_THROW(guardStament.execute());
      BOOST_REQUIRE(guardStament.fetch());
      BOOST_REQUIRE_EQUAL(count.counter->getValue(), 10);
   }

   {
      dbms::GuardConnection guardConnection(connection);
      dbms::GuardStatement guardStament(guardConnection, select.statement);

      BOOST_REQUIRE_NO_THROW(guardStament.execute());

      int counter = 0;
      while (guardStament.fetch()) {
         BOOST_REQUIRE_EQUAL(select.id->getValue(), counter);
         BOOST_REQUIRE_CLOSE(select.theFloat->getValue(), 10.11 * counter, 0.1);
         BOOST_REQUIRE_EQUAL(select.theDate->getValue(), now + counter);
         BOOST_REQUIRE(select.theBlob->getValue() == adt::DataBlock((const char*) &now, sizeof(now)));
         counter ++;
      }

      BOOST_REQUIRE_EQUAL(counter, 10);
   }
}
