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
#include <coffee/app/ApplicationServiceStarter.hpp>

#include <functional>
#include <mutex>
#include <thread>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TraceMethod.hpp>
#include <coffee/logger/TtyWriter.hpp>

#include <coffee/xml/Node.hpp>

#include "MockDatabase.hpp"
#include "MockLowLevelRecord.hpp"
#include "MockDatabaseFixture.hpp"

#include <coffee/dbms/Statement.hpp>
#include <coffee/dbms/GuardConnection.hpp>
#include <coffee/dbms/GuardStatement.hpp>
#include <coffee/dbms/StatementTranslator.hpp>
#include <coffee/dbms/FailRecoveryHandler.hpp>
#include <coffee/dbms/Database.hpp>

#include <coffee/dbms/datatype/Integer.hpp>
#include <coffee/dbms/datatype/String.hpp>
#include <coffee/dbms/datatype/Float.hpp>
#include <coffee/dbms/datatype/Date.hpp>

namespace coffee {

namespace test_dbms {

static const int IdToThrowDbException = 666;

using namespace coffee::dbms;

class MyStatement : public Statement {
protected:
   MyStatement(const Database& database, const char* name, const char* expression, const ActionOnError::_v actionOnError) :
      Statement(database, name, expression, actionOnError)
   {
      m_datas.push_back(std::make_shared<datatype::Integer>("ID"));
      m_datas.push_back(std::make_shared<datatype::String>("name", 64));
      m_datas.push_back(std::make_shared<datatype::Integer>("integer", datatype::Constraint::CanBeNull));
      m_datas.push_back(std::make_shared<datatype::Float>("float"));
      m_datas.push_back(std::make_shared<datatype::Date>("time", datatype::Constraint::CanBeNull));
   }

   mock::MockLowLevelRecord& getRecord() noexcept { return std::ref(m_record); }

protected:
   std::vector<std::shared_ptr<datatype::Abstract> > m_datas;

private:
   mock::MockLowLevelRecord m_record;
   void do_prepare(dbms::Connection& connection) throw(adt::RuntimeException, DatabaseException) {;}
};

class MyReadStatement : public MyStatement {
public:
   MyReadStatement(const Database& database, const char* name, const char* expression, const ActionOnError::_v actionOnError);

private:
   std::vector <mock::MockLowLevelRecord> m_selection;
   int m_index;

   ResultCode do_execute(Connection& connection) throw(adt::RuntimeException, DatabaseException);
   bool do_fetch() throw(adt::RuntimeException, DatabaseException);
};

class MyWriteStatement : public MyStatement {
public:
   MyWriteStatement(const Database& database, const char* name, const char* expression, const ActionOnError::_v actionOnError);

private:
   ResultCode do_execute(Connection& connection) throw(adt::RuntimeException, DatabaseException);
   bool do_fetch() throw(adt::RuntimeException, DatabaseException) { return false; }
};

class MyDatabase : public mock::MockDatabase {
public:
   explicit MyDatabase(app::Application& app) : mock::MockDatabase(app) {;}

   static std::shared_ptr<MyDatabase> instantiate(app::Application& app) noexcept {
      auto result = std::make_shared<MyDatabase>(app);
      app.attach(result);
      return result;
   }

private:
   mock::MockLowLevelContainer m_container;

   void do_stop() throw(adt::RuntimeException) {;}

   std::shared_ptr<Statement> allocateStatement(const char* name, const std::string& expression, const ActionOnError::_v actionOnError)
      throw(adt::RuntimeException)
   {
      std::shared_ptr<Statement> result;

      if(expression == "read" || expression == "READ")
         result = std::make_shared<MyReadStatement>(*this, name, expression.c_str(), actionOnError);

      if(expression == "write" || expression == "delete")
         result = std::make_shared<MyWriteStatement>(*this, name, expression.c_str(), actionOnError);

      if (!result)
         COFFEE_THROW_EXCEPTION(name << " invalid statement");

      return result;
   }
};

class MyTranslator : public dbms::StatementTranslator {
public:
   MyTranslator() : dbms::StatementTranslator("MyTranslator"), m_buffer(NULL) {;}
   ~MyTranslator() { if(m_buffer != NULL) free(m_buffer); }

private:
   char* m_buffer;

   const char* apply(const char* statement) throw(adt::RuntimeException);
};

class MyRecoveryHandler : public dbms::FailRecoveryHandler {
public:
   MyRecoveryHandler() : m_thisWasUsed(0) {;}

   int thisWasUsed() const noexcept { return m_thisWasUsed; }

private:
   int m_thisWasUsed;

   void apply(const dbms::Connection& connection) throw(adt::RuntimeException) {
      ++ m_thisWasUsed;
   }
};

}
}

using namespace coffee;
using namespace coffee::dbms;
using namespace coffee::mock;

test_dbms::MyReadStatement::MyReadStatement(const Database& database, const char* name, const char* expression, const ActionOnError::_v actionOnError) :
   MyStatement(database, name, expression, actionOnError),
   m_index(0)
{
   createBinderInput(m_datas[0]);

   int index = 0;
   for (auto member : m_datas) {
      if (index > 0) {
         createBinderOutput(member);
      }
      ++ index;
   }
}

dbms::ResultCode test_dbms::MyReadStatement::do_execute(dbms::Connection& connection)
   throw(adt::RuntimeException, DatabaseException)
{
   LOG_THIS_METHOD();

   m_index = 0;

   mock::MockConnection& _connection(static_cast <mock::MockConnection&>(connection));

   m_selection.clear();
   for(auto ii : _connection.getContainer()) {

      if(ii.second.m_id >= coffee_datatype_downcast(dbms::datatype::Integer, m_datas[0])->getValue()) {
         m_selection.push_back(ii.second);
      }
   }

   LOG_DEBUG("n-size:" << m_selection.size());

   return ResultCode(getDatabase(),(m_selection.size() == 0) ? MyDatabase::NotFound: MyDatabase::Successful);
}

bool test_dbms::MyReadStatement::do_fetch() throw(adt::RuntimeException, DatabaseException)
{
   LOG_THIS_METHOD();

   if(m_index >= m_selection.size())
      return false;

   const MockLowLevelRecord& record = m_selection [m_index ++];

   coffee_datatype_downcast(dbms::datatype::Integer, m_datas[0])->setValue(record.m_id);
   coffee_datatype_downcast(dbms::datatype::String, m_datas[1])->setValue(record.m_name);

   auto integer = coffee_datatype_downcast(dbms::datatype::Integer, m_datas[2]);
   
   if(record.m_integer == -1)
      integer->isNull();
   else
      integer->setValue(record.m_integer);

   coffee_datatype_downcast(dbms::datatype::Float, m_datas[3])->setValue(record.m_float);

   auto time = coffee_datatype_downcast(dbms::datatype::Date, m_datas[4]);

   if(record.m_time == std::chrono::seconds::zero())
      time->isNull();
   else
      time->setValue(record.m_time);

   return true;
}

test_dbms::MyWriteStatement::MyWriteStatement(const Database& database, const char* name, const char* expression, const ActionOnError::_v actionOnError) :
   MyStatement(database, name, expression, actionOnError)
{
   for (auto member : m_datas) {
      createBinderInput(member);
   }
}

dbms::ResultCode test_dbms::MyWriteStatement::do_execute(dbms::Connection& connection)
   throw(adt::RuntimeException, DatabaseException)
{
   LOG_THIS_METHOD();

   dbms::ResultCode result(connection.getDatabase(), MyDatabase::Successful);

   if(connection.isAvailable() == false) {
      LOG_DEBUG(connection << " | Connection is not available");
      return dbms::ResultCode(connection.getDatabase(), MyDatabase::LostConnection);
   }

   mock::MockConnection::OpCode opCode =(getExpression() == "write") ? mock::MockConnection::Write: mock::MockConnection::Delete;

   MockLowLevelRecord record;

   record.m_id = coffee_datatype_downcast(dbms::datatype::Integer, m_datas[0])->getValue();

   if(record.m_id == test_dbms::IdToThrowDbException) {
      return dbms::ResultCode(connection.getDatabase(), MyDatabase::NotFound);
   }

   if(opCode != mock::MockConnection::Delete) {
      record.m_name = coffee_datatype_downcast(dbms::datatype::String, m_datas[1])->getValue();

      auto integer = coffee_datatype_downcast(dbms::datatype::Integer, m_datas[2]);
      record.m_integer =(integer->hasValue()) ? integer->getValue(): -1;

      record.m_float = coffee_datatype_downcast(dbms::datatype::Float, m_datas[3])->getValue();

      auto time = coffee_datatype_downcast(dbms::datatype::Date, m_datas[4]);
      record.m_time = (time->hasValue()) ? time->getValue(): std::chrono::seconds::zero();
   }

   LOG_DEBUG("ID = " << record.m_id);

   static_cast <mock::MockConnection&>(connection).addOperation(opCode, record);

   return result;
}

const char* test_dbms::MyTranslator::apply(const char* statement)
   throw(adt::RuntimeException)
{
   if(m_buffer != NULL) {
      free(m_buffer);
      m_buffer = NULL;
   }

   m_buffer = strdup(statement);
   char* ww = m_buffer;

   while(*ww) {
      *ww = toupper(*ww);
      ++ ww;
   }

   return m_buffer;
}

BOOST_AUTO_TEST_CASE(dbms_define_structure)
{
   app::ApplicationServiceStarter application("dbms_define_structure");

   auto database = test_dbms::MyDatabase::instantiate(application);

   const coffee::dbms::ConnectionParameters zero("0", "0");
   auto connection = database->createConnection("0", zero);

   BOOST_REQUIRE_THROW(database->createConnection("0", coffee::dbms::ConnectionParameters ("bis00", "bis0")), adt::RuntimeException);

   auto findConnection = database->findConnection("0");

   BOOST_REQUIRE_EQUAL(connection.get(), findConnection.get());

   BOOST_REQUIRE_THROW(database->findConnection("zzzz"), adt::RuntimeException);

   auto statement = database->createStatement("one", "write");

   BOOST_REQUIRE_THROW(database->createStatement("one", "write"), adt::RuntimeException);
   BOOST_REQUIRE_THROW(database->createStatement("the_null", "null"), adt::RuntimeException);

   auto findStatement = database->findStatement("one");

   BOOST_REQUIRE_EQUAL(statement.get(), findStatement.get());

   BOOST_REQUIRE_THROW(database->findStatement("zzzz"), adt::RuntimeException);

   adt::StreamString xxx = connection->operator coffee::adt::StreamString();

   BOOST_REQUIRE_NE(xxx.find("CommitCounter"), std::string::npos);
}

BOOST_AUTO_TEST_CASE(dbms_translator)
{
   app::ApplicationServiceStarter application("dbms_define_structure");

   auto database = test_dbms::MyDatabase::instantiate(application);

   auto st0 = database->createStatement("zero", "read");
   BOOST_REQUIRE_EQUAL(st0->getExpression(), "read");

   database->setStatementTranslator(std::make_shared<test_dbms::MyTranslator>());
   auto st1 = database->createStatement("one", "read");
   BOOST_REQUIRE_EQUAL(st1->getExpression(), "READ");

   std::string str = database->asString();
   BOOST_REQUIRE(str.find("MyTranslator") != std::string::npos);
}

struct DbmsDefineAndRun : public MockDatabaseFixture<test_dbms::MyDatabase> {
   DbmsDefineAndRun() : MockDatabaseFixture<test_dbms::MyDatabase>("Definition_test") {
      mockConnection = std::dynamic_pointer_cast<mock::MockConnection>(connection);
   }

   dbms::ResultCode  writeRecord(dbms::GuardStatement& writer, const int id) throw (dbms::DatabaseException) {
      adt::StreamString name("the ");
      name << id;

      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(id);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue(name);
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(id * id);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue((float) id * 1.1);
      char buffer[64];
      sprintf(buffer, "%d/%d/%dT%02d:%02d", 1 + id % 28, 1 + id % 11, 2000 + (id % 25), id % 12, id % 60);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue(buffer, "%d/%m/%YT%H:%M");
      return writer.execute();
   }

   void populate(std::shared_ptr<dbms::Statement>& statement) throw (dbms::DatabaseException) {
      if(true) {
         dbms::GuardConnection guard(connection);
         dbms::GuardStatement writer(guard, statement);

         BOOST_REQUIRE_EQUAL(guard.getCountLinkedStatement(), 1);

         BOOST_REQUIRE_NO_THROW(writeRecord(writer, 2));
         BOOST_REQUIRE_NO_THROW(writeRecord(writer, 5));
         BOOST_REQUIRE_NO_THROW(writeRecord(writer, 6));

         BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 3);
         BOOST_REQUIRE_EQUAL(database->container_size(), 0);
      }

      BOOST_REQUIRE_EQUAL(mockConnection->getCommitCounter(), 1);
      BOOST_REQUIRE_EQUAL(mockConnection->getRollbackCounter(), 0);
   }

   std::shared_ptr<mock::MockConnection> mockConnection;
};

BOOST_FIXTURE_TEST_CASE(dbms_write_and_read, DbmsDefineAndRun)
{
   auto stWriter = database->createStatement("the_write", "write");
   auto stReader = database->createStatement("the_read", "read");
   ResultCode resultCode;

   try {
      dbms::GuardConnection guard(connection);
      dbms::GuardStatement writer(guard, stWriter);

      BOOST_REQUIRE_EQUAL(guard.getCountLinkedStatement(), 1);

      BOOST_REQUIRE_THROW(writer.getInputData(10), adt::RuntimeException);

      BOOST_REQUIRE_NO_THROW(writeRecord(writer, 2));
      BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 1);
      BOOST_REQUIRE_EQUAL(database->container_size(), 0);

      BOOST_REQUIRE_NO_THROW(writeRecord(writer, 5));
      BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 2);
      BOOST_REQUIRE_EQUAL(database->container_size(), 0);

      BOOST_REQUIRE_NO_THROW(resultCode = writeRecord(writer, 6));
      BOOST_REQUIRE_EQUAL(resultCode.getNumericCode(), test_dbms::MyDatabase::Successful);
      BOOST_REQUIRE_EQUAL(resultCode.successful(), true);
      BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 3);
      BOOST_REQUIRE_EQUAL(database->container_size(), 0);
   }
   catch(adt::Exception& ex) {
      logger::Logger::write(ex);
      BOOST_REQUIRE_EQUAL(true, false);
   }

   BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 0);
   BOOST_REQUIRE_EQUAL(database->container_size(), 3);
   BOOST_REQUIRE_EQUAL(mockConnection->getCommitCounter(), 1);

   if(true) {
      dbms::GuardConnection guard(connection);

      dbms::GuardStatement reader(guard, stReader);
      BOOST_REQUIRE_EQUAL(guard.getCountLinkedStatement(), 1);

      auto id = coffee_datatype_downcast(datatype::Integer, reader.getInputData(0));
      const auto integer = coffee_datatype_downcast(datatype::Integer, reader.getOutputData(1));

      id->setValue(5);
      BOOST_REQUIRE_NO_THROW(resultCode = reader.execute());

      BOOST_REQUIRE_EQUAL(resultCode.successful(), true);

      int counter = 0;

      try {
      while(reader.fetch() == true) {
         ++ counter;
         BOOST_REQUIRE_EQUAL(integer->hasValue(), true);
         BOOST_REQUIRE_EQUAL(id->getValue() * id->getValue(), integer->getValue());
      }
      }
      catch(adt::Exception& ex) {
         std::cout << ex.what() << std::endl;
      }

      BOOST_REQUIRE_EQUAL(counter, 2);

      id = coffee_datatype_downcast(datatype::Integer, reader.getInputData(0));
      id->setValue(1111);

      resultCode = reader.execute();
      BOOST_REQUIRE_EQUAL(resultCode.successful(), false);
      BOOST_REQUIRE_EQUAL(resultCode.notFound(), true);
      BOOST_REQUIRE_EQUAL(reader.fetch(), false);
   }

   BOOST_REQUIRE_EQUAL(database->container_size(), 3);
   BOOST_REQUIRE_EQUAL(mockConnection->getCommitCounter(), 1); // There was not commit due to reader operation
}

BOOST_FIXTURE_TEST_CASE(dbms_write_and_delete, DbmsDefineAndRun)
{
   auto stWriter = database->createStatement("the_write", "write");
   auto stEraser = database->createStatement("the_erase", "delete");
   ResultCode resultCode;

   BOOST_REQUIRE_NO_THROW(populate(stWriter));

   BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 0);
   BOOST_REQUIRE_EQUAL(database->container_size(), 3);
   BOOST_REQUIRE_EQUAL(mockConnection->getCommitCounter(), 1);

   BOOST_REQUIRE_EQUAL(database->container_size(), 3);
   BOOST_REQUIRE_EQUAL(mockConnection->getCommitCounter(), 1); // There was not commit due to reader operation

   if(true) {
      dbms::GuardConnection guard(connection);
      dbms::GuardStatement eraser(guard, stEraser);

      auto id = coffee_datatype_downcast(datatype::Integer, eraser.getInputData(0));

      id->setValue(6);
      resultCode = eraser.execute();
      BOOST_REQUIRE_EQUAL(resultCode.successful(), true);

      BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 1);
      BOOST_REQUIRE_EQUAL(database->container_size(), 3);

      id->setValue(2);
      resultCode = eraser.execute();
      BOOST_REQUIRE_EQUAL(resultCode.successful(), true);

      BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 2);
      BOOST_REQUIRE_EQUAL(database->container_size(), 3);
   }

   BOOST_REQUIRE_EQUAL(database->container_size(), 1);
   BOOST_REQUIRE_EQUAL(mockConnection->getCommitCounter(), 2);
}

BOOST_FIXTURE_TEST_CASE(dbms_write_rollback, DbmsDefineAndRun)
{
   auto rollbackWriter = database->createStatement("rollback_write", "write");
   ResultCode resultCode;

   BOOST_REQUIRE_NO_THROW(populate(rollbackWriter));

   if(true) {
      dbms::GuardConnection guard(connection);
      dbms::GuardStatement writer(guard, rollbackWriter);

      BOOST_REQUIRE_NO_THROW(writeRecord(writer, 8));
      BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 1);
      BOOST_REQUIRE_EQUAL(database->container_size(), 3);

      BOOST_REQUIRE_THROW(writeRecord(writer, test_dbms::IdToThrowDbException), dbms::DatabaseException);
   }

   BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 0);
   BOOST_REQUIRE_EQUAL(database->container_size(), 3);
   BOOST_REQUIRE_EQUAL(mockConnection->getCommitCounter(), 1);
   BOOST_REQUIRE_EQUAL(mockConnection->getRollbackCounter(), 1);
}


BOOST_FIXTURE_TEST_CASE(dbms_write_norollback, DbmsDefineAndRun)
{
   auto noRollbackWriter = database->createStatement("no_rollback_write", "write", dbms::ActionOnError::Ignore);
   ResultCode resultCode;

   BOOST_REQUIRE_NO_THROW(populate(noRollbackWriter));

   if(true) {
      dbms::GuardConnection guard(connection);
      dbms::GuardStatement writer(guard, noRollbackWriter);

      BOOST_REQUIRE_NO_THROW(writeRecord(writer, 9));
      BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 1);
      BOOST_REQUIRE_EQUAL(database->container_size(), 3);

      BOOST_REQUIRE_NO_THROW(resultCode = writeRecord(writer, test_dbms::IdToThrowDbException));

      BOOST_REQUIRE_EQUAL(resultCode.successful(), false);
      BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 1);
      BOOST_REQUIRE_EQUAL(database->container_size(), 3);
   }

   BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 0);
   BOOST_REQUIRE_EQUAL(database->container_size(), 4);
   BOOST_REQUIRE_EQUAL(mockConnection->getCommitCounter(), 2);
   BOOST_REQUIRE_EQUAL(mockConnection->getRollbackCounter(), 0);
}

BOOST_FIXTURE_TEST_CASE(dbms_erase_rollback, DbmsDefineAndRun)
{
   auto stWriter = database->createStatement("writer", "write");
   auto rollbackEraser = database->createStatement("rollback_eraser", "delete");
   auto noRollbackEraser = database->createStatement("no_rollback_eraser", "delete", dbms::ActionOnError::Ignore);
   ResultCode resultCode;

   BOOST_REQUIRE_NO_THROW(populate(stWriter));

   if(true) {
      dbms::GuardConnection guard(connection);
      dbms::GuardStatement eraser(guard, rollbackEraser);

      coffee_datatype_downcast(datatype::Integer, eraser.getInputData(0))->setValue(6);
      eraser.execute();

      BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 1);
      BOOST_REQUIRE_EQUAL(database->container_size(), 3);

      coffee_datatype_downcast(datatype::Integer, eraser.getInputData(0))->setValue(test_dbms::IdToThrowDbException);
      BOOST_REQUIRE_THROW(eraser.execute(), dbms::DatabaseException);

      BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 1);
      BOOST_REQUIRE_EQUAL(database->container_size(), 3);
   }

   BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 0);
   BOOST_REQUIRE_EQUAL(database->container_size(), 3);
   BOOST_REQUIRE_EQUAL(mockConnection->getCommitCounter(), 1);
   BOOST_REQUIRE_EQUAL(mockConnection->getRollbackCounter(), 1);
}

BOOST_FIXTURE_TEST_CASE(dbms_erase_norollback, DbmsDefineAndRun)
{
   auto stWriter = database->createStatement("writer", "write");
   auto noRollbackEraser = database->createStatement("no_rollback_eraser", "delete", dbms::ActionOnError::Ignore);
   ResultCode resultCode;

   BOOST_REQUIRE_NO_THROW(populate(stWriter));

   if(true) {
      dbms::GuardConnection guard(connection);
      dbms::GuardStatement eraser(guard, noRollbackEraser);

      coffee_datatype_downcast(datatype::Integer, eraser.getInputData(0))->setValue(2);
      eraser.execute();

      BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 1);
      BOOST_REQUIRE_EQUAL(database->container_size(), 3);

      coffee_datatype_downcast(datatype::Integer, eraser.getInputData(0))->setValue(test_dbms::IdToThrowDbException);
      BOOST_REQUIRE_NO_THROW(resultCode = eraser.execute());

      BOOST_REQUIRE_EQUAL(resultCode.successful(), false);
      BOOST_REQUIRE_EQUAL(resultCode.notFound(), true);
      BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 1);
      BOOST_REQUIRE_EQUAL(database->container_size(), 3);
   }

   BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 0);
   BOOST_REQUIRE_EQUAL(database->container_size(), 2);
   BOOST_REQUIRE_EQUAL(mockConnection->getCommitCounter(), 2);
   BOOST_REQUIRE_EQUAL(mockConnection->getRollbackCounter(), 0);
}

BOOST_FIXTURE_TEST_CASE(dbms_set_max_commit, DbmsDefineAndRun)
{
   auto stWriter = database->createStatement("the_write", "write");
   auto stReader = database->createStatement("the_read", "read");

   if(true) {
      GuardConnection guard(connection);
      GuardStatement writer(guard, stWriter);

      BOOST_REQUIRE_EQUAL(guard.setMaxCommitPending(5), 0);

      for(int ii = 1; ii <= 15; ++ ii) {
         BOOST_REQUIRE_NO_THROW(writeRecord(writer, ii));
      }

      BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 0);
      BOOST_REQUIRE_EQUAL(mockConnection->getCommitCounter(), 3);
      BOOST_REQUIRE_EQUAL(mockConnection->getRollbackCounter(), 0);
   }

   BOOST_REQUIRE_EQUAL(database->container_size(), 15);

   if(true) {
      GuardConnection guard(connection);
      GuardStatement writer(guard, stWriter);

      guard.clearMaxCommitPending();

      for(int ii = 16; ii <= 30; ++ ii) {
         BOOST_REQUIRE_NO_THROW(writeRecord(writer, ii));
      }
   }

   BOOST_REQUIRE_EQUAL(mockConnection->getCommitCounter(), 4);
   BOOST_REQUIRE_EQUAL(mockConnection->getRollbackCounter(), 0);
   BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 0);
   BOOST_REQUIRE_EQUAL(database->container_size(), 30);
}

BOOST_FIXTURE_TEST_CASE(dbms_break_detected_executing, DbmsDefineAndRun)
{
   auto stWriter = database->createStatement("the_write", "write");

   if(true) {
      GuardConnection guard(connection);
      GuardStatement writer(guard, stWriter);

      BOOST_REQUIRE_EQUAL(mockConnection->getOpenCounter(), 1);
      BOOST_REQUIRE_EQUAL(mockConnection->getCloseCounter(), 0);

      BOOST_REQUIRE_NO_THROW(writeRecord(writer, 1));

      mockConnection->manualBreak();

      // It will detect the lost connection but it will try to recover and it will get it.
      BOOST_REQUIRE_THROW(writeRecord(writer, 2),  dbms::DatabaseException);

      BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 0);
      BOOST_REQUIRE_EQUAL(mockConnection->getOpenCounter(), 2);
      BOOST_REQUIRE_EQUAL(mockConnection->getCloseCounter(), 1);
   }

   // It will recover the connection after it will detect
   BOOST_REQUIRE_EQUAL(mockConnection->isAvailable(), true);
   BOOST_REQUIRE_EQUAL(database->container_size(), 0);
}

BOOST_FIXTURE_TEST_CASE(dbms_break_detected_while_locking, DbmsDefineAndRun)
{
   auto stWriter = database->createStatement("the_write", "write");

   if(true) {
      GuardConnection guard(connection);
      GuardStatement writer(guard, stWriter);

      BOOST_REQUIRE_EQUAL(mockConnection->getOpenCounter(), 1);
      BOOST_REQUIRE_EQUAL(mockConnection->getCloseCounter(), 0);

      BOOST_REQUIRE_NO_THROW(writeRecord(writer, 1));
   }

   mockConnection->manualBreak();

   if(true) {
      // It will detect the lost connection but it will try to recover and it will get it.
      GuardConnection guard(connection);
      GuardStatement writer(guard, stWriter);

      BOOST_REQUIRE_EQUAL(mockConnection->getOpenCounter(), 2);
      BOOST_REQUIRE_EQUAL(mockConnection->getCloseCounter(), 1);

      BOOST_REQUIRE_NO_THROW(writeRecord(writer, 2));
   }

   // It will recover the connection after it will detect
   BOOST_REQUIRE_EQUAL(mockConnection->isAvailable(), true);
   BOOST_REQUIRE_EQUAL(database->container_size(), 2);

   if(true) {
      // It will detect the lost connection but it will try to recover and it will get it.
      GuardConnection guard(connection);

      mockConnection->manualBreak();

      BOOST_REQUIRE_THROW(GuardStatement writer(guard, stWriter), adt::RuntimeException);

      BOOST_REQUIRE_EQUAL(mockConnection->getOpenCounter(), 2);
      BOOST_REQUIRE_EQUAL(mockConnection->getCloseCounter(), 1);
   }

   if(true) {
      GuardConnection guard(connection);
      GuardStatement writer(guard, stWriter);

      BOOST_REQUIRE_EQUAL(mockConnection->getOpenCounter(), 3);
      BOOST_REQUIRE_EQUAL(mockConnection->getCloseCounter(), 2);

      BOOST_REQUIRE_NO_THROW(writeRecord(writer, 3));
   }

   // It will recover the connection after it will detect
   BOOST_REQUIRE_EQUAL(mockConnection->isAvailable(), true);
   BOOST_REQUIRE_EQUAL(database->container_size(), 3);
}

BOOST_FIXTURE_TEST_CASE(dbms_break_unrecovery_executing, DbmsDefineAndRun)
{
   std::shared_ptr<test_dbms::MyRecoveryHandler> recoveryHandler = std::make_shared<test_dbms::MyRecoveryHandler>();
   database->setFailRecoveryHandler(recoveryHandler);

   auto stWriter = database->createStatement("the_write", "write");

   if(true) {
      GuardConnection guard(connection);
      GuardStatement writer(guard, stWriter);

      BOOST_REQUIRE_EQUAL(mockConnection->getOpenCounter(), 1);
      BOOST_REQUIRE_EQUAL(mockConnection->getCloseCounter(), 0);

      BOOST_REQUIRE_NO_THROW(writeRecord(writer, 1));

      mockConnection->manualBreak();
      mockConnection->avoidRecovery();

      // It will detect the lost connection but it will try to recover and it will get it.
      BOOST_REQUIRE_THROW(writeRecord(writer, 2), dbms::DatabaseException);

      BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 0);
      BOOST_REQUIRE_EQUAL(mockConnection->getOpenCounter(), 1);
      BOOST_REQUIRE_EQUAL(mockConnection->getCloseCounter(), 1);
      BOOST_REQUIRE_EQUAL(recoveryHandler->thisWasUsed(), 1);
   }

   // It will recover the connection after it will detect
   BOOST_REQUIRE_EQUAL(mockConnection->isAvailable(), false);
   BOOST_REQUIRE_EQUAL(database->container_size(), 0);
}

BOOST_FIXTURE_TEST_CASE(dbms_break_unrecovery_while_locking, DbmsDefineAndRun)
{
   std::shared_ptr<test_dbms::MyRecoveryHandler> recoveryHandler = std::make_shared<test_dbms::MyRecoveryHandler>();
   database->setFailRecoveryHandler(recoveryHandler);

   auto stWriter = database->createStatement("the_write", "write");

   if(true) {
      GuardConnection guard(connection);
      GuardStatement writer(guard, stWriter);

      BOOST_REQUIRE_EQUAL(mockConnection->getOpenCounter(), 1);
      BOOST_REQUIRE_EQUAL(mockConnection->getCloseCounter(), 0);

      BOOST_REQUIRE_NO_THROW(writeRecord(writer, 1));
   }

   mockConnection->manualBreak();
   mockConnection->avoidRecovery();

   if(true) {
      BOOST_REQUIRE_THROW(GuardConnection guard(connection), adt::RuntimeException);
      BOOST_REQUIRE_EQUAL(mockConnection->getOpenCounter(), 1);
      BOOST_REQUIRE_EQUAL(mockConnection->getCloseCounter(), 1);
      BOOST_REQUIRE_EQUAL(recoveryHandler->thisWasUsed(), 1);
   }

   // It will recover the connection after it will detect
   BOOST_REQUIRE_EQUAL(mockConnection->isAvailable(), false);
   BOOST_REQUIRE_EQUAL(database->container_size(), 1);
}

BOOST_FIXTURE_TEST_CASE(dbms_dealing_with_nulls, DbmsDefineAndRun)
{
   auto stWriter = database->createStatement("the_write", "write");
   auto stReader = database->createStatement("the_read", "read");
   ResultCode resultCode;

   if(true) {
      try {
         dbms::GuardConnection guard(connection);
         dbms::GuardStatement writer(guard, stWriter);

         coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(20);
         coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the 20");
         coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->isNull();
         coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(20.20);
         coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue("2/2/2002T02:02:02", "%d/%m/%YT%H:%M");
         writer.execute();

         BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 1);
         BOOST_REQUIRE_EQUAL(database->container_size(), 0);

         coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(25);
         coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the 25");
         coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(25 * 25);
         coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(25.25);
         coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->isNull();
         ResultCode resultCode = writer.execute();

         BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 2);
         BOOST_REQUIRE_EQUAL(database->container_size(), 0);

         BOOST_REQUIRE_EQUAL(resultCode.getNumericCode(), test_dbms::MyDatabase::Successful);
         BOOST_REQUIRE_EQUAL(resultCode.successful(), true);
         BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 2);
         BOOST_REQUIRE_EQUAL(database->container_size(), 0);
      }
      catch(adt::Exception& ex) {
         logger::Logger::write(ex);
      }
   }

   BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 0);
   BOOST_REQUIRE_EQUAL(database->container_size(), 2);
   BOOST_REQUIRE_EQUAL(mockConnection->getCommitCounter(), 1);

   if(true) {
      dbms::GuardConnection guard(connection);

      dbms::GuardStatement reader(guard, stReader);
      BOOST_REQUIRE_EQUAL(guard.getCountLinkedStatement(), 1);

      auto id = coffee_datatype_downcast(datatype::Integer, reader.getInputData(0));
      auto integer = coffee_datatype_downcast(datatype::Integer, reader.getOutputData(1));
      auto date = coffee_datatype_downcast(datatype::Date, reader.getOutputData(3));

      id->setValue(0);
      resultCode = reader.execute();

      BOOST_REQUIRE_EQUAL(resultCode.successful(), true);

      BOOST_REQUIRE_EQUAL(reader.fetch(), true);

      BOOST_REQUIRE_EQUAL(id->getValue(), 20);
      BOOST_REQUIRE_EQUAL(integer->hasValue(), false);
      BOOST_REQUIRE_THROW(integer->getValue(), adt::RuntimeException);
      BOOST_REQUIRE_EQUAL(date->hasValue(), true);

      BOOST_REQUIRE_EQUAL(reader.fetch(), true);

      BOOST_REQUIRE_EQUAL(id->getValue(), 25);
      BOOST_REQUIRE_EQUAL(integer->hasValue(), true);
      BOOST_REQUIRE_EQUAL(date->hasValue(), false);
      BOOST_REQUIRE_THROW(date->getValue(), adt::RuntimeException);

      BOOST_REQUIRE_EQUAL(reader.fetch(), false);
   }
}

BOOST_AUTO_TEST_CASE(dbms_null_binder_allocated)
{
   app::ApplicationServiceStarter application("dbms_null_binder_allocated");
   auto database = test_dbms::MyDatabase::instantiate(application);
   auto statement = database->createStatement("one", "read");
   auto id = std::make_shared<datatype::Integer>("give-me-null");
   BOOST_REQUIRE_THROW(statement->createBinderInput(id), adt::RuntimeException);
   BOOST_REQUIRE_THROW(statement->createBinderOutput(id), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(dbms_input_binder_out_range)
{
   app::ApplicationServiceStarter application("dbms_input_binder_out_range");
   auto database = test_dbms::MyDatabase::instantiate(application);
   auto stWriter = database->createStatement("the_write", "write");
   auto stReader = database->createStatement("the_read", "read");
   auto connection = database->createConnection("0", coffee::dbms::ConnectionParameters ("0", "0"));

   dbms::GuardConnection guard(connection);
   dbms::GuardStatement writer(guard, stWriter);
   BOOST_REQUIRE_THROW(writer.getInputData(100), adt::RuntimeException);

   dbms::GuardStatement reader(guard, stReader);
   BOOST_REQUIRE_THROW(reader.getOutputData(100), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(dbms_resultcode_without_interpreter)
{
   app::ApplicationServiceStarter application("dbms_resultcode_without_interpreter");
   auto database = test_dbms::MyDatabase::instantiate(application);
   std::shared_ptr<dbms::ErrorCodeInterpreter> empty;
   database->setErrorCodeInterpreter(empty);
   dbms::ResultCode resultCode(*database, 100);

   BOOST_REQUIRE_THROW(resultCode.successful(), adt::RuntimeException);
   BOOST_REQUIRE_THROW(resultCode.notFound(), adt::RuntimeException);
   BOOST_REQUIRE_THROW(resultCode.locked(), adt::RuntimeException);
   BOOST_REQUIRE_THROW(resultCode.lostConnection(), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(dbms_resultcode_asstring)
{
   app::ApplicationServiceStarter application("dbms_resultcode_asstring");
   auto database = test_dbms::MyDatabase::instantiate(application);
   dbms::ResultCode resultCode(*database, 100, "text");

   BOOST_REQUIRE_EQUAL(resultCode.asString(), "dbms.ResultCode { Status=(100) | Comment=text }");
}

BOOST_AUTO_TEST_CASE(dbms_database_asXML)
{
   app::ApplicationServiceStarter application("dbms_database_asXML");
   auto database = test_dbms::MyDatabase::instantiate(application);

   database->createConnection("connection0", coffee::dbms::ConnectionParameters ("user0", "password0"));
   database->createStatement("the_write", "write");
   database->createStatement("the_read", "read");

   auto node = std::make_shared<xml::Node>("root");
   std::shared_ptr<xml::Node> result = database->asXML(node);

   BOOST_REQUIRE_EQUAL(result->children_size(), 3);
}

BOOST_FIXTURE_TEST_CASE(dbms_noauto_commit, DbmsDefineAndRun)
{
   auto stWriter = database->createStatement("the_write", "write");
   ResultCode resultCode;

   if (true) {
      dbms::GuardConnection guard(connection);
      dbms::GuardStatement writer(guard, stWriter);

      BOOST_REQUIRE_EQUAL(guard.getCountLinkedStatement(), 1);

      BOOST_REQUIRE_NO_THROW(writeRecord(writer, 2));

      BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 1);
      BOOST_REQUIRE_EQUAL(database->container_size(), 0);
      BOOST_REQUIRE_EQUAL(mockConnection->getCommitPendingCounter(), 1);
   }

   BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 0);
   BOOST_REQUIRE_EQUAL(database->container_size(), 1);
   BOOST_REQUIRE_EQUAL(mockConnection->getCommitPendingCounter(), 0);

   if (true) {
      dbms::GuardConnection guard(connection);
      dbms::GuardStatement writer(guard, stWriter);

      // Avoiding commit will be rejected due to sentence with input parameter => it is an insert
      BOOST_REQUIRE(writer.setRequiresCommit(false));

      BOOST_REQUIRE_NO_THROW(writeRecord(writer, 3));

      BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 1);
      BOOST_REQUIRE_EQUAL(mockConnection->getCommitPendingCounter(), 1);
   }

   BOOST_REQUIRE_EQUAL(mockConnection->operation_size(), 0);
   BOOST_REQUIRE_EQUAL(database->container_size(), 2);
   BOOST_REQUIRE_EQUAL(mockConnection->getCommitCounter(), 2);
}
