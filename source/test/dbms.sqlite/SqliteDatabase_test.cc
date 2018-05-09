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

#include <memory>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TtyWriter.hpp>

#include <coffee/time/TimeService.hpp>

#include <coffee/app/ApplicationServiceStarter.hpp>

#include <coffee/dbms.sqlite/SqliteDatabase.hpp>
#include <coffee/dbms.sqlite/SqliteConnection.hpp>

#include <coffee/dbms/GuardConnection.hpp>
#include <coffee/dbms/GuardStatement.hpp>
#include <coffee/dbms/Connection.hpp>
#include <coffee/dbms/ConnectionParameters.hpp>
#include <coffee/dbms/Statement.hpp>
#include <coffee/dbms/datatype/String.hpp>
#include <coffee/dbms/datatype/Integer.hpp>
#include <coffee/dbms/datatype/Float.hpp>
#include <coffee/dbms/datatype/LongBlock.hpp>
#include <coffee/dbms/datatype/ShortBlock.hpp>
#include <coffee/dbms/datatype/Date.hpp>
#include <coffee/dbms/datatype/TimeStamp.hpp>
#include <coffee/logger/UnlimitedTraceWriter.hpp>

#include "../dbms/PrintChrono.hpp"

using namespace coffee;

using std::chrono::seconds;

struct StatementAgeGreater {
   StatementAgeGreater(std::shared_ptr<dbms::Database> database, std::shared_ptr<dbms::Connection>& connection) {
      const char* sql = "select Name, age from employee where age > ?";

      inputId = std::make_shared<dbms::datatype::Integer>("age");
      outputId = std::make_shared<dbms::datatype::Integer>("age");
      outputName = std::make_shared<dbms::datatype::String>("name", 20);

      statement = database->createStatement("greater_age", sql);
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
   StatementCountAgeGreater(std::shared_ptr<dbms::Database> database, std::shared_ptr<dbms::Connection>& connection) {
      const char* sql = "select count(*) from employee where age > ?";

      inputId = std::make_shared<dbms::datatype::Integer>("age");
      outputId = std::make_shared<dbms::datatype::Integer>("count");

      statement = database->createStatement("count_greater_age", sql);
      statement->createBinderInput(inputId);
      statement->createBinderOutput(outputId);
   }
   std::shared_ptr<dbms::datatype::Integer> inputId;
   std::shared_ptr<dbms::datatype::Integer> outputId;
   std::shared_ptr<dbms::Statement> statement;
};

struct StatementCountAllTypes {
   StatementCountAllTypes(std::shared_ptr<dbms::Database> database, std::shared_ptr<dbms::Connection>& connection) {
      const char* sql = "select count(*) from all_types";
      counter = std::make_shared<dbms::datatype::Integer>("counter");
      statement = database->createStatement("count_all_types", sql);
      statement->createBinderOutput(counter);
   }
   std::shared_ptr<dbms::datatype::Integer> counter;
   std::shared_ptr<dbms::Statement> statement;
};

struct StatementBlob {
   StatementBlob(std::shared_ptr<dbms::Database> database, std::shared_ptr<dbms::Connection>& connection) {
      const char* sql = "select id, myfloat, mydata from btest where id == ?";

      id = std::make_shared<dbms::datatype::Integer>("id");
      myfloat = std::make_shared<dbms::datatype::Float>("float");
      mydata = std::make_shared<dbms::datatype::LongBlock>("data", dbms::datatype::Constraint::CanBeNull);

      statement = database->createStatement("select_blob", sql);
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
   StatementSyntaxError(std::shared_ptr<dbms::Database> database, std::shared_ptr<dbms::Connection>& connection) {
      const char* sql = "select age where employee where age > ?";
      input = std::make_shared<dbms::datatype::Integer>("age");
      output = std::make_shared<dbms::datatype::Integer>("age");
      statement = database->createStatement("syntax_error", sql);
      statement->createBinderInput(input);
      statement->createBinderOutput(output);
   };
   std::shared_ptr<dbms::datatype::Integer> input;
   std::shared_ptr<dbms::datatype::Integer> output;
   std::shared_ptr<dbms::Statement> statement;
};

struct InsertAllType {
   InsertAllType(std::shared_ptr<dbms::Database> database, std::shared_ptr<dbms::Connection>& connection) {
      const char* sql = "insert into all_types(id, the_float, the_date, the_blob, the_time, the_long_blob) values (?, ?, ?, ?, ?, ?)";
      id = std::make_shared<dbms::datatype::Integer>("id");
      theFloat = std::make_shared<dbms::datatype::Float>("the_float", dbms::datatype::Constraint::CanBeNull);
      theDate = std::make_shared<dbms::datatype::Date>("the_date", dbms::datatype::Constraint::CanBeNull);
      theBlob = std::make_shared<dbms::datatype::ShortBlock>("the_blob", 128, dbms::datatype::Constraint::CanBeNull);
      theTime = std::make_shared<dbms::datatype::TimeStamp>("the_time", dbms::datatype::Constraint::CanBeNull);
      theLongBlob = std::make_shared<dbms::datatype::LongBlock>("the_long_blob", dbms::datatype::Constraint::CanBeNull);
      statement = database->createStatement("insert_all_types", sql);
      statement->createBinderInput(id);
      statement->createBinderInput(theFloat);
      statement->createBinderInput(theDate);
      statement->createBinderInput(theBlob);
      statement->createBinderInput(theTime);
      statement->createBinderInput(theLongBlob);
   }

   std::shared_ptr<dbms::datatype::Integer> id;
   std::shared_ptr<dbms::datatype::Float> theFloat;
   std::shared_ptr<dbms::datatype::Date> theDate;
   std::shared_ptr<dbms::datatype::ShortBlock> theBlob;
   std::shared_ptr<dbms::datatype::TimeStamp> theTime;
   std::shared_ptr<dbms::datatype::LongBlock> theLongBlob;
   std::shared_ptr<dbms::Statement> statement;
};

struct SelectAllType {
   SelectAllType(std::shared_ptr<dbms::Database> database, std::shared_ptr<dbms::Connection>& connection) {
      const char* sql = "select id, the_float, the_date, the_blob, the_time, the_long_blob from all_types order by id";
      id = std::make_shared<dbms::datatype::Integer>("id");
      theFloat = std::make_shared<dbms::datatype::Float>("the_float", dbms::datatype::Constraint::CanBeNull);
      theDate = std::make_shared<dbms::datatype::Date>("the_date", dbms::datatype::Constraint::CanBeNull);
      theBlob = std::make_shared<dbms::datatype::ShortBlock>("the_blob", 128, dbms::datatype::Constraint::CanBeNull);
      theTime = std::make_shared<dbms::datatype::TimeStamp>("the_time", dbms::datatype::Constraint::CanBeNull);
      theLongBlob = std::make_shared<dbms::datatype::LongBlock>("the_long_blob", dbms::datatype::Constraint::CanBeNull);
      statement = database->createStatement("select_all_types", sql);
      statement->createBinderOutput(id);
      statement->createBinderOutput(theFloat);
      statement->createBinderOutput(theDate);
      statement->createBinderOutput(theBlob);
      statement->createBinderOutput(theTime);
      statement->createBinderOutput(theLongBlob);
   }

   std::shared_ptr<dbms::datatype::Integer> id;
   std::shared_ptr<dbms::datatype::Float> theFloat;
   std::shared_ptr<dbms::datatype::Date> theDate;
   std::shared_ptr<dbms::datatype::ShortBlock> theBlob;
   std::shared_ptr<dbms::datatype::TimeStamp> theTime;
   std::shared_ptr<dbms::datatype::LongBlock> theLongBlob;
   std::shared_ptr<dbms::Statement> statement;
};

static void sqliteParallelRun(coffee::app::Application& app) {
   app.start();
}

struct SqliteFixture  {
   static  boost::filesystem::path dbPath;

   SqliteFixture() : app ("TestSqliteApplication"){
      const char* logFileName = "source/test/dbms.sqlite/trace.log";
      unlink (logFileName);
      logger::Logger::initialize(std::make_shared<logger::UnlimitedTraceWriter>(logFileName));
      logger::Logger::setLevel(logger::Level::Debug);

      database = dbms::sqlite::SqliteDatabase::instantiate(app, dbPath);
      thr = std::thread(sqliteParallelRun, std::ref(app));
      app.waitUntilRunning();
      BOOST_REQUIRE(database->isRunning());
      boost::filesystem::remove(dbPath);
      connection = database->createConnection("first", coffee::dbms::ConnectionParameters("user:first", "none"));
      BOOST_REQUIRE_EQUAL(boost::filesystem::exists(dbPath), true);
      BOOST_REQUIRE_EQUAL(connection->isAvailable(), true);

      const char* sql = " \
         BEGIN TRANSACTION; \
         CREATE TABLE employee (Name varchar(20),Dept varchar(20),jobTitle varchar(20), age integer); \
         create table btest(ID INTEGER, myfloat float, MyData BLOB); \
         create table all_types (id integer primary key, the_float float, the_date date, the_blob blob, the_time time, the_long_blob blob); \
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
      app.stop();
      thr.join();
   }

   app::ApplicationServiceStarter app;
   std::shared_ptr<dbms::sqlite::SqliteDatabase> database;
   std::thread thr;
   std::shared_ptr<dbms::Connection> connection;
};

boost::filesystem::path SqliteFixture::dbPath("/tmp/sqlite_test.db");

BOOST_FIXTURE_TEST_CASE(sqlite_create_db, SqliteFixture)
{
   auto secondConnection = database->createConnection("second", coffee::dbms::ConnectionParameters("user:second", "none"));
   BOOST_REQUIRE_EQUAL(secondConnection->isAvailable(), true);
}

struct SqliteFixtureBadPath  {
   static  boost::filesystem::path dbBadPath;

   SqliteFixtureBadPath() : app ("TestSqliteApplication-Badpath"){
      database = dbms::sqlite::SqliteDatabase::instantiate(app, dbBadPath);
      thr = std::thread(sqliteParallelRun, std::ref(app));
      app.waitUntilRunning();
   }

   ~SqliteFixtureBadPath() {
      app.stop();
      thr.join();
   }

   app::ApplicationServiceStarter app;
   std::shared_ptr<dbms::sqlite::SqliteDatabase> database;
   std::thread thr;
};

boost::filesystem::path SqliteFixtureBadPath::dbBadPath("/root");

BOOST_FIXTURE_TEST_CASE(sqlite_connection_badpath, SqliteFixtureBadPath)
{
   BOOST_REQUIRE(database->isRunning());
   BOOST_REQUIRE_THROW(database->createConnection("db_is_not_working", coffee::dbms::ConnectionParameters ("user:first", "none")), dbms::DatabaseException);
}

BOOST_FIXTURE_TEST_CASE(undefined_column_select, SqliteFixture)
{
   const char* sql = "select no-column from all_types";
   dbms::GuardConnection guardConnection(connection);
   BOOST_REQUIRE_THROW(std::dynamic_pointer_cast<dbms::sqlite::SqliteConnection>(connection)->execute(sql), dbms::DatabaseException);
}

BOOST_FIXTURE_TEST_CASE(undefined_table_select, SqliteFixture)
{
   const char* sql = "select no-column from no_table";
   dbms::GuardConnection guardConnection(connection);
   BOOST_REQUIRE_THROW(std::dynamic_pointer_cast<dbms::sqlite::SqliteConnection>(connection)->execute(sql), dbms::DatabaseException);
}

BOOST_FIXTURE_TEST_CASE(bad_syntax, SqliteFixture)
{
   const char* sql = "select no-column where no_table";
   dbms::GuardConnection guardConnection(connection);
   BOOST_REQUIRE_THROW(std::dynamic_pointer_cast<dbms::sqlite::SqliteConnection>(connection)->execute(sql), dbms::DatabaseException);
}

BOOST_FIXTURE_TEST_CASE(bad_input_parameters, SqliteFixture)
{
   const char* sql = "select * from all_types where id = ?";
   auto statement = database->createStatement("bad_input_parameters", sql);
   dbms::GuardConnection guardConnection(connection);
   dbms::GuardStatement guardStament(guardConnection, statement);
   BOOST_REQUIRE_THROW(guardStament.execute(), basis::RuntimeException);
}

BOOST_FIXTURE_TEST_CASE(bad_output_parameters, SqliteFixture)
{
   const char* sql = "select id from all_types where id = ?";
   auto statement = database->createStatement("bad_output_parameters", sql);
   auto id = std::make_shared<dbms::datatype::Integer>("id");
   statement->createBinderInput(id);
   dbms::GuardConnection guardConnection(connection);
   dbms::GuardStatement guardStament(guardConnection, statement);
   BOOST_REQUIRE_THROW(guardStament.execute(), basis::RuntimeException);
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

   auto insert = database->createStatement("insert_employee", sql);
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

   auto secondConnection = database->createConnection("second", coffee::dbms::ConnectionParameters("user:second", "none"));

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

   const basis::DataBlock& dbBlob = fullStatement.mydata->getValue();
   basis::DataBlock expected("123456789-12345", 15);
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

   auto secondConnection = database->createConnection("second", coffee::dbms::ConnectionParameters("user:second", "none"));

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
      auto now = time::TimeService::toSeconds(time::TimeService::now());

      dbms::GuardConnection guardConnection(connection);
      dbms::GuardStatement guardStament(guardConnection, insert.statement);
      insert.id->setValue(100);
      insert.theBlob->isNull();
      insert.theFloat->isNull();
      insert.theDate->setValue(now);
      insert.theLongBlob->isNull();
      insert.theTime->setValue(now);
      BOOST_REQUIRE_NO_THROW(guardStament.execute());
   }

   {
      dbms::GuardConnection guardConnection(connection);
      dbms::GuardStatement guardStament(guardConnection, count.statement);
      BOOST_REQUIRE_NO_THROW(guardStament.execute());
      BOOST_REQUIRE(guardStament.fetch());
      BOOST_REQUIRE_EQUAL(count.counter->getValue(), 1);
   }

   { // Try to repeat the primary Key
      dbms::GuardConnection guardConnection(connection);
      dbms::GuardStatement guardStament(guardConnection, insert.statement);
      insert.id->setValue(100);
      BOOST_REQUIRE_THROW(guardStament.execute(), dbms::DatabaseException);
   }
}

BOOST_FIXTURE_TEST_CASE(sqlite_insert_all_types, SqliteFixture)
{
   auto now = time::TimeService::toSeconds(time::TimeService::now());

   InsertAllType insert(database, connection);
   StatementCountAllTypes count(database, connection);
   SelectAllType select(database, connection);

   {
      dbms::GuardConnection guardConnection(connection);
      dbms::GuardStatement guardStament(guardConnection, insert.statement);
      char buffer[1024];

      for (int ii = 0; ii < 10; ++ ii) {
         insert.id->setValue(ii);
         insert.theFloat->setValue(10.11 * ii);
         insert.theDate->setValue(now + seconds(ii));
         insert.theBlob->setValue(basis::DataBlock((const char*) &now, sizeof(now)));
         insert.theTime->setValue(now + seconds(ii * 2));
         memset(buffer, now.count() % 255, sizeof(buffer));
         basis::DataBlock value(buffer, sizeof(buffer));
         insert.theLongBlob->setValue(value);
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
         BOOST_REQUIRE_EQUAL(select.theDate->getValue(), now + seconds(counter));
         BOOST_REQUIRE(select.theBlob->getValue() == basis::DataBlock((const char*) &now, sizeof(now)));
         BOOST_REQUIRE(select.theTime->getValue() == now + seconds(counter * 2));
         counter ++;
      }

      BOOST_REQUIRE_EQUAL(counter, 10);
   }
}

