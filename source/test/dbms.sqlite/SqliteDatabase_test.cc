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

#include <gtest/gtest.h>
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

struct SqliteDatabaseTestFixture  : public ::testing::Test {
   static  boost::filesystem::path dbPath;

   SqliteDatabaseTestFixture() : app ("TestSqliteApplication"){}
   
   void SetUp() {
      const char* logFileName = "source/test/dbms.sqlite/trace.log";
      unlink (logFileName);
      logger::Logger::initialize(std::make_shared<logger::UnlimitedTraceWriter>(logFileName));
      logger::Logger::setLevel(logger::Level::Debug);

      database = dbms::sqlite::SqliteDatabase::instantiate(app, dbPath);
      thr = std::thread(sqliteParallelRun, std::ref(app));
      app.waitUntilRunning();
      ASSERT_TRUE(database->isRunning());
      boost::filesystem::remove(dbPath);
      connection = database->createConnection("first", coffee::dbms::ConnectionParameters("user:first", "none"));
      ASSERT_TRUE(boost::filesystem::exists(dbPath));
      ASSERT_TRUE(connection->isAvailable());

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

      ASSERT_NO_THROW(std::dynamic_pointer_cast<dbms::sqlite::SqliteConnection>(connection)->execute(sql));
   }
   
   ~SqliteDatabaseTestFixture() {
      app.stop();
      thr.join();
   }

   app::ApplicationServiceStarter app;
   std::shared_ptr<dbms::sqlite::SqliteDatabase> database;
   std::thread thr;
   std::shared_ptr<dbms::Connection> connection;
};

boost::filesystem::path SqliteDatabaseTestFixture::dbPath("/tmp/sqlite_test.db");

TEST_F(SqliteDatabaseTestFixture, create_db)
{
   auto secondConnection = database->createConnection("second", coffee::dbms::ConnectionParameters("user:second", "none"));
   ASSERT_TRUE(secondConnection->isAvailable());
}

TEST_F(SqliteDatabaseTestFixture, undefined_column_select)
{
   const char* sql = "select no-column from all_types";
   dbms::GuardConnection guardConnection(connection);
   ASSERT_THROW(std::dynamic_pointer_cast<dbms::sqlite::SqliteConnection>(connection)->execute(sql), dbms::DatabaseException);
}

TEST_F(SqliteDatabaseTestFixture, undefined_table_select)
{
   const char* sql = "select no-column from no_table";
   dbms::GuardConnection guardConnection(connection);
   ASSERT_THROW(std::dynamic_pointer_cast<dbms::sqlite::SqliteConnection>(connection)->execute(sql), dbms::DatabaseException);
}

TEST_F(SqliteDatabaseTestFixture, bad_syntax)
{
   const char* sql = "select no-column where no_table";
   dbms::GuardConnection guardConnection(connection);
   ASSERT_THROW(std::dynamic_pointer_cast<dbms::sqlite::SqliteConnection>(connection)->execute(sql), dbms::DatabaseException);
}

TEST_F(SqliteDatabaseTestFixture, bad_input_parameters)
{
   const char* sql = "select * from all_types where id = ?";
   auto statement = database->createStatement("bad_input_parameters", sql);
   dbms::GuardConnection guardConnection(connection);
   dbms::GuardStatement guardStament(guardConnection, statement);
   ASSERT_THROW(guardStament.execute(), basis::RuntimeException);
}

TEST_F(SqliteDatabaseTestFixture, bad_output_parameters)
{
   const char* sql = "select id from all_types where id = ?";
   auto statement = database->createStatement("bad_output_parameters", sql);
   auto id = std::make_shared<dbms::datatype::Integer>("id");
   statement->createBinderInput(id);
   dbms::GuardConnection guardConnection(connection);
   dbms::GuardStatement guardStament(guardConnection, statement);
   ASSERT_THROW(guardStament.execute(), basis::RuntimeException);
}

// See http://www.yolinux.com/TUTORIALS/SQLite.html
TEST_F(SqliteDatabaseTestFixture, multi_select)
{
   StatementAgeGreater fullStatement(database, connection);
   dbms::GuardConnection guardConnection(connection);
   dbms::GuardStatement guardStament(guardConnection, fullStatement.statement);

   fullStatement.inputId->setValue(40);

   dbms::ResultCode resultCode = guardStament.execute();

   ASSERT_TRUE(resultCode.successful());

   int counter = 0;
   while(guardStament.fetch()) {
      ASSERT_GT(fullStatement.outputId->getValue(), 40);
      counter ++;
   }

   ASSERT_EQ(2, counter);
}

TEST_F(SqliteDatabaseTestFixture, unique_select)
{
   StatementAgeGreater fullStatement(database, connection);

   dbms::GuardConnection guardConnection(connection);
   dbms::GuardStatement guardStament(guardConnection, fullStatement.statement);

   fullStatement.inputId->setValue(50);

   dbms::ResultCode resultCode = guardStament.execute();

   ASSERT_TRUE(resultCode.successful());

   int counter = 0;
   while(guardStament.fetch()) {
      ASSERT_EQ(60, fullStatement.outputId->getValue());
      ASSERT_EQ(std::string("Betty Rubble"), fullStatement.outputName->getValue());
      counter ++;
   }

   ASSERT_EQ(1, counter);
}

TEST_F(SqliteDatabaseTestFixture, no_select)
{
   StatementAgeGreater fullStatement(database, connection);

   dbms::GuardConnection guardConnection(connection);
   dbms::GuardStatement guardStament(guardConnection, fullStatement.statement);

   fullStatement.inputId->setValue(100);

   dbms::ResultCode resultCode = guardStament.execute();

   ASSERT_TRUE(resultCode.notFound());
   ASSERT_TRUE(!guardStament.fetch());
}

TEST_F(SqliteDatabaseTestFixture, insert)
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

      ASSERT_TRUE(rc.successful());
   }

   auto secondConnection = database->createConnection("second", coffee::dbms::ConnectionParameters("user:second", "none"));

   {
      dbms::GuardConnection guardConnection(secondConnection);
      dbms::GuardStatement guardStament(guardConnection, ageCounter.statement);
      ageCounter.inputId->setValue(30);
      dbms::ResultCode resultCode = guardStament.execute();
      ASSERT_TRUE(resultCode.successful());
      ASSERT_TRUE(guardStament.fetch());
      ASSERT_EQ(3 + 1, ageCounter.outputId->getValue());
   }
}

TEST_F(SqliteDatabaseTestFixture, float)
{
   StatementBlob fullStatement(database, connection);

   dbms::GuardConnection guardConnection(connection);
   dbms::GuardStatement guardStament(guardConnection, fullStatement.statement);
   fullStatement.id->setValue(11);
   dbms::ResultCode resultCode = guardStament.execute();
   ASSERT_TRUE(resultCode.successful());
   ASSERT_TRUE(guardStament.fetch());
   ASSERT_FLOAT_EQ(0.11, fullStatement.myfloat->getValue());
   ASSERT_TRUE(!fullStatement.mydata->hasValue());

   fullStatement.id->setValue(22);
   resultCode = guardStament.execute();
   ASSERT_TRUE(resultCode.successful());
   ASSERT_TRUE(guardStament.fetch());
   ASSERT_FLOAT_EQ(0.22, fullStatement.myfloat->getValue());

   const basis::DataBlock& dbBlob = fullStatement.mydata->getValue();
   basis::DataBlock expected("123456789-12345", 15);
   ASSERT_EQ(expected.size(), dbBlob.size());
   ASSERT_EQ(0, memcmp(dbBlob.data(), expected.data(), expected.size()));
}

TEST_F(SqliteDatabaseTestFixture, rebind_sentence)
{
   StatementCountAgeGreater fullStatement(database, connection);

   dbms::GuardConnection guardConnection(connection);
   dbms::GuardStatement guardStament(guardConnection, fullStatement.statement);

   {
      fullStatement.inputId->setValue(10);
      dbms::ResultCode resultCode = guardStament.execute();
      ASSERT_TRUE(resultCode.successful());
      ASSERT_TRUE(guardStament.fetch());
      ASSERT_EQ(4, fullStatement.outputId->getValue());
   }

   {
      fullStatement.inputId->setValue(40);
      dbms::ResultCode resultCode = guardStament.execute();
      ASSERT_TRUE(resultCode.successful());
      ASSERT_TRUE(guardStament.fetch());
      ASSERT_EQ(2, fullStatement.outputId->getValue());
   }

   {
      fullStatement.inputId->setValue(100);
      dbms::ResultCode resultCode = guardStament.execute();
      ASSERT_TRUE(resultCode.successful());
      ASSERT_TRUE(guardStament.fetch());
      ASSERT_EQ(0, fullStatement.outputId->getValue());
   }
}

TEST_F(SqliteDatabaseTestFixture, reuse_sentence)
{
   StatementCountAgeGreater ageCounter(database, connection);

   {
      dbms::GuardConnection guardConnection(connection);
      dbms::GuardStatement guardStament(guardConnection, ageCounter.statement);

      ageCounter.inputId->setValue(10);
      dbms::ResultCode resultCode = guardStament.execute();
      ASSERT_TRUE(resultCode.successful());
      ASSERT_TRUE(guardStament.fetch());
      ASSERT_EQ(4, ageCounter.outputId->getValue());
   }

   auto secondConnection = database->createConnection("second", coffee::dbms::ConnectionParameters("user:second", "none"));

   {
      dbms::GuardConnection guardConnection(secondConnection);
      dbms::GuardStatement guardStament(guardConnection, ageCounter.statement);

      ageCounter.inputId->setValue(30);
      dbms::ResultCode resultCode = guardStament.execute();
      ASSERT_TRUE(resultCode.successful());
      ASSERT_TRUE(guardStament.fetch());
      ASSERT_EQ(3, ageCounter.outputId->getValue());
   }
}

TEST_F(SqliteDatabaseTestFixture, syntax_error)
{
   StatementSyntaxError syntaxError(database, connection);

   dbms::GuardConnection guardConnection(connection);
   dbms::GuardStatement guardStament(guardConnection, syntaxError.statement);
   ASSERT_THROW(guardStament.execute(), dbms::DatabaseException);
}


TEST_F(SqliteDatabaseTestFixture, rollback)
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
      ASSERT_NO_THROW(guardStament.execute());
   }

   {
      dbms::GuardConnection guardConnection(connection);
      dbms::GuardStatement guardStament(guardConnection, count.statement);
      ASSERT_NO_THROW(guardStament.execute());
      ASSERT_TRUE(guardStament.fetch());
      ASSERT_EQ(1, count.counter->getValue());
   }

   { // Try to repeat the primary Key
      dbms::GuardConnection guardConnection(connection);
      dbms::GuardStatement guardStament(guardConnection, insert.statement);
      insert.id->setValue(100);
      ASSERT_THROW(guardStament.execute(), dbms::DatabaseException);
   }
}

TEST_F(SqliteDatabaseTestFixture, insert_all_types)
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
         ASSERT_NO_THROW(guardStament.execute());
      }
   }

   {
      dbms::GuardConnection guardConnection(connection);
      dbms::GuardStatement guardStament(guardConnection, count.statement);
      ASSERT_NO_THROW(guardStament.execute());
      ASSERT_TRUE(guardStament.fetch());
      ASSERT_EQ(10, count.counter->getValue());
   }

   {
      dbms::GuardConnection guardConnection(connection);
      dbms::GuardStatement guardStament(guardConnection, select.statement);

      ASSERT_NO_THROW(guardStament.execute());

      int counter = 0;
      while (guardStament.fetch()) {
         ASSERT_EQ(counter, select.id->getValue());
         ASSERT_FLOAT_EQ(10.11 * counter, select.theFloat->getValue());
         ASSERT_EQ(now + seconds(counter), select.theDate->getValue());
         ASSERT_TRUE(select.theBlob->getValue() == basis::DataBlock((const char*) &now, sizeof(now)));
         ASSERT_TRUE(select.theTime->getValue() == now + seconds(counter * 2));
         counter ++;
      }

      ASSERT_EQ(10, counter);
   }
}

struct SqliteDatabaseTestFixtureBadPath : public ::testing::Test {
   static  boost::filesystem::path dbBadPath;

   SqliteDatabaseTestFixtureBadPath() : app ("TestSqliteApplication-Badpath"){
      database = dbms::sqlite::SqliteDatabase::instantiate(app, dbBadPath);
      thr = std::thread(sqliteParallelRun, std::ref(app));
      app.waitUntilRunning();
   }

   ~SqliteDatabaseTestFixtureBadPath() {
      app.stop();
      thr.join();
   }

   app::ApplicationServiceStarter app;
   std::shared_ptr<dbms::sqlite::SqliteDatabase> database;
   std::thread thr;
};

boost::filesystem::path SqliteDatabaseTestFixtureBadPath::dbBadPath("/root");

TEST_F(SqliteDatabaseTestFixtureBadPath, cannot_connect)
{
   ASSERT_TRUE(database->isRunning());
   ASSERT_THROW(database->createConnection("db_is_not_working", coffee::dbms::ConnectionParameters ("user:first", "none")), dbms::DatabaseException);
}
