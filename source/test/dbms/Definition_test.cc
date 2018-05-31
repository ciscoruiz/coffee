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
   MyStatement(const Database& database, const char* name, const char* expression, const StatementParameters& parameters) :
      Statement(database, name, expression, parameters)
   {
      m_datas.push_back(datatype::Integer::instantiate("ID"));
      m_datas.push_back(datatype::String::instantiate("name", 64));
      m_datas.push_back(datatype::Integer::instantiate("integer", datatype::Constraint::CanBeNull));
      m_datas.push_back(datatype::Float::instantiate("float"));
      m_datas.push_back(datatype::Date::instantiate("time", datatype::Constraint::CanBeNull));
   }

   mock::MockLowLevelRecord& getRecord() noexcept { return std::ref(m_record); }

protected:
   std::vector<std::shared_ptr<datatype::Abstract> > m_datas;

private:
   mock::MockLowLevelRecord m_record;
   void do_prepare(dbms::Connection& connection) throw(basis::RuntimeException, DatabaseException) {;}
};

class MyReadStatement : public MyStatement {
public:
   MyReadStatement(const Database& database, const char* name, const char* expression, const StatementParameters& parameters);

private:
   std::vector <mock::MockLowLevelRecord> m_selection;
   int m_index;

   ResultCode do_execute(Connection& connection) throw(basis::RuntimeException, DatabaseException);
   bool do_fetch() throw(basis::RuntimeException, DatabaseException);
};

class MyWriteStatement : public MyStatement {
public:
   MyWriteStatement(const Database& database, const char* name, const char* expression, const StatementParameters& parameters);

private:
   ResultCode do_execute(Connection& connection) throw(basis::RuntimeException, DatabaseException);
   bool do_fetch() throw(basis::RuntimeException, DatabaseException) { return false; }
};

class MyDatabase : public mock::MockDatabase {
public:
   explicit MyDatabase(app::Application& app) : mock::MockDatabase(app) {;}

   static std::shared_ptr<MyDatabase> instantiate(app::Application& app) throw(basis::RuntimeException) {
      auto result = std::make_shared<MyDatabase>(app);
      app.attach(result);
      return result;
   }

private:
   mock::MockLowLevelContainer m_container;

   void do_stop() throw(basis::RuntimeException) {;}

   std::shared_ptr<Statement> allocateStatement(const char* name, const std::string& expression, const StatementParameters& parameters)
      throw(basis::RuntimeException)
   {
      std::shared_ptr<Statement> result;

      if(expression == "read" || expression == "READ")
         result = std::make_shared<MyReadStatement>(*this, name, expression.c_str(), parameters);

      if(expression == "write" || expression == "delete")
         result = std::make_shared<MyWriteStatement>(*this, name, expression.c_str(), parameters);

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

   const char* apply(const char* statement) throw(basis::RuntimeException);
};

class MyRecoveryHandler : public dbms::FailRecoveryHandler {
public:
   MyRecoveryHandler() : m_thisWasUsed(0) {;}

   int thisWasUsed() const noexcept { return m_thisWasUsed; }

private:
   int m_thisWasUsed;

   void apply(const dbms::Connection& connection) throw(basis::RuntimeException) {
      ++ m_thisWasUsed;
   }
};

}
}

using namespace coffee;
using namespace coffee::dbms;
using namespace coffee::mock;

test_dbms::MyReadStatement::MyReadStatement(const Database& database, const char* name, const char* expression, const StatementParameters& parameters) :
   MyStatement(database, name, expression, parameters),
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
   throw(basis::RuntimeException, DatabaseException)
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

bool test_dbms::MyReadStatement::do_fetch() throw(basis::RuntimeException, DatabaseException)
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

test_dbms::MyWriteStatement::MyWriteStatement(const Database& database, const char* name, const char* expression, const StatementParameters& parameters) :
   MyStatement(database, name, expression, parameters)
{
   for (auto member : m_datas) {
      createBinderInput(member);
   }
}

dbms::ResultCode test_dbms::MyWriteStatement::do_execute(dbms::Connection& connection)
   throw(basis::RuntimeException, DatabaseException)
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
   throw(basis::RuntimeException)
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

TEST(DbmsDefinitionTest, creates)
{
   app::ApplicationServiceStarter application("define_structure");

   auto database = test_dbms::MyDatabase::instantiate(application);

   ASSERT_TRUE(database != nullptr);

   const coffee::dbms::ConnectionParameters zero("0", "0");
   auto connection = database->createConnection("0", zero);

   ASSERT_THROW(database->createConnection("0", coffee::dbms::ConnectionParameters ("bis00", "bis0")), basis::RuntimeException);

   auto findConnection = database->findConnection("0");

   ASSERT_EQ(findConnection.get(), connection.get());

   ASSERT_THROW(database->findConnection("zzzz"), basis::RuntimeException);

   auto statement = database->createStatement("one", "write");

   ASSERT_THROW(database->createStatement("one", "write"), basis::RuntimeException);
   ASSERT_THROW(database->createStatement("the_null", "null"), basis::RuntimeException);

   auto findStatement = database->findStatement("one");

   ASSERT_EQ(findStatement.get(), statement.get());

   ASSERT_THROW(database->findStatement("zzzz"), basis::RuntimeException);

   basis::StreamString xxx = connection->operator coffee::basis::StreamString();

   ASSERT_NE(xxx.find("CommitCounter"), std::string::npos);
}

TEST(DbmsDefinitionTest, translator)
{
   app::ApplicationServiceStarter application("define_structure");

   auto database = test_dbms::MyDatabase::instantiate(application);

   auto st0 = database->createStatement("zero", "read");
   ASSERT_EQ("read", st0->getExpression());

   database->setStatementTranslator(std::make_shared<test_dbms::MyTranslator>());
   auto st1 = database->createStatement("one", "read");
   ASSERT_EQ("READ", st1->getExpression());

   std::string str = database->asString();
   ASSERT_TRUE(str.find("MyTranslator") != std::string::npos);
}

struct DbmsDefineAndPopulateTest : public MockDatabaseFixture<test_dbms::MyDatabase> {
   DbmsDefineAndPopulateTest() : MockDatabaseFixture<test_dbms::MyDatabase>("Definition_test") { ; }

   void SetUp() {
      MockDatabaseFixture<test_dbms::MyDatabase>::SetUp();
      mockConnection = std::dynamic_pointer_cast<mock::MockConnection>(connection);
   }

   dbms::ResultCode writeRecord(dbms::GuardStatement& writer, const int id)
      throw (dbms::DatabaseException, dbms::InvalidDataException)
   {
      basis::StreamString name("the ");
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

         ASSERT_EQ(1, guard.getCountLinkedStatement());

         ASSERT_NO_THROW(writeRecord(writer, 2));
         ASSERT_NO_THROW(writeRecord(writer, 5));
         ASSERT_NO_THROW(writeRecord(writer, 6));

         ASSERT_EQ(3, mockConnection->operation_size());
         ASSERT_EQ(0, database->container_size());
      }

      ASSERT_EQ(1, mockConnection->getCommitCounter());
      ASSERT_EQ(0, mockConnection->getRollbackCounter());
   }

   std::shared_ptr<mock::MockConnection> mockConnection;
};

TEST_F(DbmsDefineAndPopulateTest,write_and_read)
{
   auto stWriter = database->createStatement("the_write", "write");
   auto stReader = database->createStatement("the_read", "read");
   ResultCode resultCode;

   try {
      dbms::GuardConnection guard(connection);
      dbms::GuardStatement writer(guard, stWriter);

      ASSERT_EQ(1, guard.getCountLinkedStatement());

      ASSERT_THROW(writer.getInputData(10), basis::RuntimeException);

      ASSERT_NO_THROW(writeRecord(writer, 2));
      ASSERT_EQ(1, mockConnection->operation_size());
      ASSERT_EQ(0, database->container_size());

      ASSERT_NO_THROW(writeRecord(writer, 5));
      ASSERT_EQ(2, mockConnection->operation_size());
      ASSERT_EQ(0, database->container_size());

      ASSERT_NO_THROW(resultCode = writeRecord(writer, 6));
      ASSERT_EQ(test_dbms::MyDatabase::Successful, resultCode.getNumericCode());
      ASSERT_TRUE(resultCode.successful());
      ASSERT_EQ(3, mockConnection->operation_size());
      ASSERT_EQ(0, database->container_size());
   }
   catch(basis::Exception& ex) {
      logger::Logger::write(ex);
      ASSERT_FALSE(true);
   }

   ASSERT_EQ(0, mockConnection->operation_size());
   ASSERT_EQ(3, database->container_size());
   ASSERT_EQ(1, mockConnection->getCommitCounter());

   if(true) {
      dbms::GuardConnection guard(connection);

      dbms::GuardStatement reader(guard, stReader);
      ASSERT_EQ(1, guard.getCountLinkedStatement());

      auto id = coffee_datatype_downcast(datatype::Integer, reader.getInputData(0));
      const auto integer = coffee_datatype_downcast(datatype::Integer, reader.getOutputData(1));

      id->setValue(5);
      ASSERT_NO_THROW(resultCode = reader.execute());

      ASSERT_TRUE(resultCode.successful());

      int counter = 0;

      try {
      while(reader.fetch() == true) {
         ++ counter;
         ASSERT_TRUE(integer->hasValue());
         ASSERT_EQ(integer->getValue(), id->getValue() * id->getValue());
      }
      }
      catch(basis::Exception& ex) {
         std::cout << ex.what() << std::endl;
      }

      ASSERT_EQ(2, counter);

      id = coffee_datatype_downcast(datatype::Integer, reader.getInputData(0));
      id->setValue(1111);

      resultCode = reader.execute();
      ASSERT_FALSE(resultCode.successful());
      ASSERT_TRUE(resultCode.notFound());
      ASSERT_FALSE(reader.fetch());
   }

   ASSERT_EQ(3, database->container_size());
   ASSERT_EQ(1, mockConnection->getCommitCounter()); // There was not commit due to reader operation
}

TEST_F(DbmsDefineAndPopulateTest,write_and_delete)
{
   auto stWriter = database->createStatement("the_write", "write");
   auto stEraser = database->createStatement("the_erase", "delete");
   ResultCode resultCode;

   ASSERT_NO_THROW(populate(stWriter));

   ASSERT_EQ(0, mockConnection->operation_size());
   ASSERT_EQ(3, database->container_size());
   ASSERT_EQ(1, mockConnection->getCommitCounter());

   ASSERT_EQ(3, database->container_size());
   ASSERT_EQ(1, mockConnection->getCommitCounter()); // There was not commit due to reader operation

   if(true) {
      dbms::GuardConnection guard(connection);
      dbms::GuardStatement eraser(guard, stEraser);

      auto id = coffee_datatype_downcast(datatype::Integer, eraser.getInputData(0));

      id->setValue(6);
      resultCode = eraser.execute();
      ASSERT_TRUE(resultCode.successful());

      ASSERT_EQ(1, mockConnection->operation_size());
      ASSERT_EQ(3, database->container_size());

      id->setValue(2);
      resultCode = eraser.execute();
      ASSERT_TRUE(resultCode.successful());

      ASSERT_EQ(2, mockConnection->operation_size());
      ASSERT_EQ(3, database->container_size());
   }

   ASSERT_EQ(1, database->container_size());
   ASSERT_EQ(2, mockConnection->getCommitCounter());
}

TEST_F(DbmsDefineAndPopulateTest,write_rollback)
{
   auto rollbackWriter = database->createStatement("rollback_write", "write");
   ResultCode resultCode;

   ASSERT_NO_THROW(populate(rollbackWriter));

   if(true) {
      dbms::GuardConnection guard(connection);
      dbms::GuardStatement writer(guard, rollbackWriter);

      ASSERT_NO_THROW(writeRecord(writer, 8));
      ASSERT_EQ(1, mockConnection->operation_size());
      ASSERT_EQ(3, database->container_size());

      ASSERT_THROW(writeRecord(writer, test_dbms::IdToThrowDbException), dbms::DatabaseException);
   }

   ASSERT_EQ(0, mockConnection->operation_size());
   ASSERT_EQ(3, database->container_size());
   ASSERT_EQ(1, mockConnection->getCommitCounter());
   ASSERT_EQ(1, mockConnection->getRollbackCounter());
}


TEST_F(DbmsDefineAndPopulateTest,write_norollback)
{
   auto noRollbackWriter = database->createStatement("no_rollback_write", "write", dbms::StatementParameters(dbms::ActionOnError::Ignore));
   ResultCode resultCode;

   ASSERT_NO_THROW(populate(noRollbackWriter));

   if(true) {
      dbms::GuardConnection guard(connection);
      dbms::GuardStatement writer(guard, noRollbackWriter);

      ASSERT_NO_THROW(writeRecord(writer, 9));
      ASSERT_EQ(1, mockConnection->operation_size());
      ASSERT_EQ(3, database->container_size());

      ASSERT_NO_THROW(resultCode = writeRecord(writer, test_dbms::IdToThrowDbException));

      ASSERT_FALSE(resultCode.successful());
      ASSERT_EQ(1, mockConnection->operation_size());
      ASSERT_EQ(3, database->container_size());
   }

   ASSERT_EQ(0, mockConnection->operation_size());
   ASSERT_EQ(4, database->container_size());
   ASSERT_EQ(2, mockConnection->getCommitCounter());
   ASSERT_EQ(0, mockConnection->getRollbackCounter());
}

TEST_F(DbmsDefineAndPopulateTest,erase_rollback)
{
   auto stWriter = database->createStatement("writer", "write");
   auto rollbackEraser = database->createStatement("rollback_eraser", "delete");
   auto noRollbackEraser = database->createStatement("no_rollback_eraser", "delete", dbms::StatementParameters(dbms::ActionOnError::Ignore));
   ResultCode resultCode;

   ASSERT_NO_THROW(populate(stWriter));

   if(true) {
      dbms::GuardConnection guard(connection);
      dbms::GuardStatement eraser(guard, rollbackEraser);

      coffee_datatype_downcast(datatype::Integer, eraser.getInputData(0))->setValue(6);
      eraser.execute();

      ASSERT_EQ(1, mockConnection->operation_size());
      ASSERT_EQ(3, database->container_size());

      coffee_datatype_downcast(datatype::Integer, eraser.getInputData(0))->setValue(test_dbms::IdToThrowDbException);
      ASSERT_THROW(eraser.execute(), dbms::DatabaseException);

      ASSERT_EQ(1, mockConnection->operation_size());
      ASSERT_EQ(3, database->container_size());
   }

   ASSERT_EQ(0, mockConnection->operation_size());
   ASSERT_EQ(3, database->container_size());
   ASSERT_EQ(1, mockConnection->getCommitCounter());
   ASSERT_EQ(1, mockConnection->getRollbackCounter());
}

TEST_F(DbmsDefineAndPopulateTest,erase_norollback)
{
   auto stWriter = database->createStatement("writer", "write");
   auto noRollbackEraser = database->createStatement("no_rollback_eraser", "delete", dbms::StatementParameters(dbms::ActionOnError::Ignore));
   ResultCode resultCode;

   ASSERT_NO_THROW(populate(stWriter));

   if(true) {
      dbms::GuardConnection guard(connection);
      dbms::GuardStatement eraser(guard, noRollbackEraser);

      coffee_datatype_downcast(datatype::Integer, eraser.getInputData(0))->setValue(2);
      eraser.execute();

      ASSERT_EQ(1, mockConnection->operation_size());
      ASSERT_EQ(3, database->container_size());

      coffee_datatype_downcast(datatype::Integer, eraser.getInputData(0))->setValue(test_dbms::IdToThrowDbException);
      ASSERT_NO_THROW(resultCode = eraser.execute());

      ASSERT_FALSE(resultCode.successful());
      ASSERT_TRUE(resultCode.notFound());
      ASSERT_EQ(1, mockConnection->operation_size());
      ASSERT_EQ(3, database->container_size());
   }

   ASSERT_EQ(0, mockConnection->operation_size());
   ASSERT_EQ(2, database->container_size());
   ASSERT_EQ(2, mockConnection->getCommitCounter());
   ASSERT_EQ(0, mockConnection->getRollbackCounter());
}

TEST_F(DbmsDefineAndPopulateTest,set_max_commit)
{
   auto stWriter = database->createStatement("the_write", "write");
   auto stReader = database->createStatement("the_read", "read");

   if(true) {
      GuardConnection guard(connection);
      GuardStatement writer(guard, stWriter);

      ASSERT_EQ(0, guard.setMaxCommitPending(5));

      for(int ii = 1; ii <= 15; ++ ii) {
         ASSERT_NO_THROW(writeRecord(writer, ii));
      }

      ASSERT_EQ(0, mockConnection->operation_size());
      ASSERT_EQ(3, mockConnection->getCommitCounter());
      ASSERT_EQ(0, mockConnection->getRollbackCounter());
   }

   ASSERT_EQ(15, database->container_size());

   if(true) {
      GuardConnection guard(connection);
      GuardStatement writer(guard, stWriter);

      guard.clearMaxCommitPending();

      for(int ii = 16; ii <= 30; ++ ii) {
         ASSERT_NO_THROW(writeRecord(writer, ii));
      }
   }

   ASSERT_EQ(4, mockConnection->getCommitCounter());
   ASSERT_EQ(0, mockConnection->getRollbackCounter());
   ASSERT_EQ(0, mockConnection->operation_size());
   ASSERT_EQ(30, database->container_size());
}

TEST_F(DbmsDefineAndPopulateTest,break_detected_executing)
{
   auto stWriter = database->createStatement("the_write", "write");

   if(true) {
      GuardConnection guard(connection);
      GuardStatement writer(guard, stWriter);

      ASSERT_EQ(1, mockConnection->getOpenCounter());
      ASSERT_EQ(0, mockConnection->getCloseCounter());

      ASSERT_NO_THROW(writeRecord(writer, 1));

      mockConnection->manualBreak();

      // It will detect the lost connection but it will try to recover and it will get it.
      ASSERT_THROW(writeRecord(writer, 2),  dbms::DatabaseException);

      ASSERT_EQ(0, mockConnection->operation_size());
      ASSERT_EQ(2, mockConnection->getOpenCounter());
      ASSERT_EQ(1, mockConnection->getCloseCounter());
   }

   // It will recover the connection after it will detect
   ASSERT_TRUE(mockConnection->isAvailable());
   ASSERT_EQ(0, database->container_size());
}

TEST_F(DbmsDefineAndPopulateTest,break_detected_while_locking)
{
   auto stWriter = database->createStatement("the_write", "write");

   if(true) {
      GuardConnection guard(connection);
      GuardStatement writer(guard, stWriter);

      ASSERT_EQ(1, mockConnection->getOpenCounter());
      ASSERT_EQ(0, mockConnection->getCloseCounter());

      ASSERT_NO_THROW(writeRecord(writer, 1));
   }

   mockConnection->manualBreak();

   if(true) {
      // It will detect the lost connection but it will try to recover and it will get it.
      GuardConnection guard(connection);
      GuardStatement writer(guard, stWriter);

      ASSERT_EQ(2, mockConnection->getOpenCounter());
      ASSERT_EQ(1, mockConnection->getCloseCounter());

      ASSERT_NO_THROW(writeRecord(writer, 2));
   }

   // It will recover the connection after it will detect
   ASSERT_TRUE(mockConnection->isAvailable());
   ASSERT_EQ(2, database->container_size());

   if(true) {
      // It will detect the lost connection but it will try to recover and it will get it.
      GuardConnection guard(connection);

      mockConnection->manualBreak();

      ASSERT_THROW(GuardStatement writer(guard, stWriter), basis::RuntimeException);

      ASSERT_EQ(2, mockConnection->getOpenCounter());
      ASSERT_EQ(1, mockConnection->getCloseCounter());
   }

   if(true) {
      GuardConnection guard(connection);
      GuardStatement writer(guard, stWriter);

      ASSERT_EQ(3, mockConnection->getOpenCounter());
      ASSERT_EQ(2, mockConnection->getCloseCounter());

      ASSERT_NO_THROW(writeRecord(writer, 3));
   }

   // It will recover the connection after it will detect
   ASSERT_TRUE(mockConnection->isAvailable());
   ASSERT_EQ(3, database->container_size());
}

TEST_F(DbmsDefineAndPopulateTest,break_unrecovery_executing)
{
   std::shared_ptr<test_dbms::MyRecoveryHandler> recoveryHandler = std::make_shared<test_dbms::MyRecoveryHandler>();
   database->setFailRecoveryHandler(recoveryHandler);

   auto stWriter = database->createStatement("the_write", "write");

   if(true) {
      GuardConnection guard(connection);
      GuardStatement writer(guard, stWriter);

      ASSERT_EQ(1, mockConnection->getOpenCounter());
      ASSERT_EQ(0, mockConnection->getCloseCounter());

      ASSERT_NO_THROW(writeRecord(writer, 1));

      mockConnection->manualBreak();
      mockConnection->avoidRecovery();

      // It will detect the lost connection but it will try to recover and it will get it.
      ASSERT_THROW(writeRecord(writer, 2), dbms::DatabaseException);

      ASSERT_EQ(0, mockConnection->operation_size());
      ASSERT_EQ(1, mockConnection->getOpenCounter());
      ASSERT_EQ(1, mockConnection->getCloseCounter());
      ASSERT_EQ(1, recoveryHandler->thisWasUsed());
   }

   // It will recover the connection after it will detect
   ASSERT_FALSE(mockConnection->isAvailable());
   ASSERT_EQ(0, database->container_size());
}

TEST_F(DbmsDefineAndPopulateTest,break_unrecovery_while_locking)
{
   std::shared_ptr<test_dbms::MyRecoveryHandler> recoveryHandler = std::make_shared<test_dbms::MyRecoveryHandler>();
   database->setFailRecoveryHandler(recoveryHandler);

   auto stWriter = database->createStatement("the_write", "write");

   if(true) {
      GuardConnection guard(connection);
      GuardStatement writer(guard, stWriter);

      ASSERT_EQ(1, mockConnection->getOpenCounter());
      ASSERT_EQ(0, mockConnection->getCloseCounter());

      ASSERT_NO_THROW(writeRecord(writer, 1));
   }

   mockConnection->manualBreak();
   mockConnection->avoidRecovery();

   if(true) {
      ASSERT_THROW(GuardConnection guard(connection), basis::RuntimeException);
      ASSERT_EQ(1, mockConnection->getOpenCounter());
      ASSERT_EQ(1, mockConnection->getCloseCounter());
      ASSERT_EQ(1, recoveryHandler->thisWasUsed());
   }

   // It will recover the connection after it will detect
   ASSERT_FALSE(mockConnection->isAvailable());
   ASSERT_EQ(1, database->container_size());
}

TEST_F(DbmsDefineAndPopulateTest,dealing_with_nulls)
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

         ASSERT_EQ(1, mockConnection->operation_size());
         ASSERT_EQ(0, database->container_size());

         coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(25);
         coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the 25");
         coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(25 * 25);
         coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(25.25);
         coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->isNull();
         ResultCode resultCode = writer.execute();

         ASSERT_EQ(2, mockConnection->operation_size());
         ASSERT_EQ(0, database->container_size());

         ASSERT_EQ(test_dbms::MyDatabase::Successful, resultCode.getNumericCode());
         ASSERT_TRUE(resultCode.successful());
         ASSERT_EQ(2, mockConnection->operation_size());
         ASSERT_EQ(0, database->container_size());
      }
      catch(basis::Exception& ex) {
         logger::Logger::write(ex);
      }
   }

   ASSERT_EQ(0, mockConnection->operation_size());
   ASSERT_EQ(2, database->container_size());
   ASSERT_EQ(1, mockConnection->getCommitCounter());

   if(true) {
      dbms::GuardConnection guard(connection);

      dbms::GuardStatement reader(guard, stReader);
      ASSERT_EQ(1, guard.getCountLinkedStatement());

      auto id = coffee_datatype_downcast(datatype::Integer, reader.getInputData(0));
      auto integer = coffee_datatype_downcast(datatype::Integer, reader.getOutputData(1));
      auto date = coffee_datatype_downcast(datatype::Date, reader.getOutputData(3));

      id->setValue(0);
      resultCode = reader.execute();

      ASSERT_TRUE(resultCode.successful());

      ASSERT_TRUE(reader.fetch());

      ASSERT_EQ(20, id->getValue());
      ASSERT_FALSE(integer->hasValue());
      ASSERT_THROW(integer->getValue(), basis::RuntimeException);
      ASSERT_TRUE(date->hasValue());

      ASSERT_TRUE(reader.fetch());

      ASSERT_EQ(25, id->getValue());
      ASSERT_TRUE(integer->hasValue());
      ASSERT_FALSE(date->hasValue());
      ASSERT_THROW(date->getValue(), basis::RuntimeException);

      ASSERT_FALSE(reader.fetch());
   }
}

TEST(DbmsDefinition, null_binder_allocated)
{
   app::ApplicationServiceStarter application("null_binder_allocated");
   auto database = test_dbms::MyDatabase::instantiate(application);
   auto statement = database->createStatement("one", "read");
   auto id = std::make_shared<datatype::Integer>("give-me-null");
   ASSERT_THROW(statement->createBinderInput(id), basis::RuntimeException);
   ASSERT_THROW(statement->createBinderOutput(id), basis::RuntimeException);
}

TEST(DbmsDefinition, input_binder_out_range)
{
   app::ApplicationServiceStarter application("input_binder_out_range");
   auto database = test_dbms::MyDatabase::instantiate(application);
   auto stWriter = database->createStatement("the_write", "write");
   auto stReader = database->createStatement("the_read", "read");
   auto connection = database->createConnection("0", coffee::dbms::ConnectionParameters ("0", "0"));

   dbms::GuardConnection guard(connection);
   dbms::GuardStatement writer(guard, stWriter);
   ASSERT_THROW(writer.getInputData(100), basis::RuntimeException);

   dbms::GuardStatement reader(guard, stReader);
   ASSERT_THROW(reader.getOutputData(100), basis::RuntimeException);
}

TEST(DbmsDefinition, resultcode_without_interpreter)
{
   app::ApplicationServiceStarter application("resultcode_without_interpreter");
   auto database = test_dbms::MyDatabase::instantiate(application);
   std::shared_ptr<dbms::ErrorCodeInterpreter> empty;
   database->setErrorCodeInterpreter(empty);
   dbms::ResultCode resultCode(*database, 100);

   ASSERT_THROW(resultCode.successful(), basis::RuntimeException);
   ASSERT_THROW(resultCode.notFound(), basis::RuntimeException);
   ASSERT_THROW(resultCode.locked(), basis::RuntimeException);
   ASSERT_THROW(resultCode.lostConnection(), basis::RuntimeException);
}

TEST(DbmsDefinition, resultcode_asstring)
{
   app::ApplicationServiceStarter application("resultcode_asstring");
   auto database = test_dbms::MyDatabase::instantiate(application);
   dbms::ResultCode resultCode(*database, 100, "text");

   ASSERT_EQ("dbms.ResultCode { Status=(100) | Comment=text }", resultCode.asString());
}

TEST(DbmsDefinition, database_asXML)
{
   app::ApplicationServiceStarter application("database_asXML");
   auto database = test_dbms::MyDatabase::instantiate(application);

   database->createConnection("connection0", coffee::dbms::ConnectionParameters ("user0", "password0"));
   database->createStatement("the_write", "write");
   database->createStatement("the_read", "read");

   auto node = std::make_shared<xml::Node>("root");
   std::shared_ptr<xml::Node> result = database->asXML(node);

   ASSERT_EQ(3, result->children_size());
}

TEST_F(DbmsDefineAndPopulateTest,noauto_commit)
{
   auto stWriter = database->createStatement("the_write", "write");
   ResultCode resultCode;

   if (true) {
      dbms::GuardConnection guard(connection);
      dbms::GuardStatement writer(guard, stWriter);

      ASSERT_EQ(1, guard.getCountLinkedStatement());

      ASSERT_NO_THROW(writeRecord(writer, 2));

      ASSERT_EQ(1, mockConnection->operation_size());
      ASSERT_EQ(0, database->container_size());
      ASSERT_EQ(1, mockConnection->getCommitPendingCounter());
   }

   ASSERT_EQ(0, mockConnection->operation_size());
   ASSERT_EQ(1, database->container_size());
   ASSERT_EQ(0, mockConnection->getCommitPendingCounter());

   if (true) {
      dbms::GuardConnection guard(connection);
      dbms::GuardStatement writer(guard, stWriter);

      // Avoiding commit will be rejected due to sentence with input parameter => it is an insert
      ASSERT_TRUE(writer.setRequiresCommit(false));

      ASSERT_NO_THROW(writeRecord(writer, 3));

      ASSERT_EQ(1, mockConnection->operation_size());
      ASSERT_EQ(1, mockConnection->getCommitPendingCounter());
   }

   ASSERT_EQ(0, mockConnection->operation_size());
   ASSERT_EQ(2, database->container_size());
   ASSERT_EQ(2, mockConnection->getCommitCounter());
}
