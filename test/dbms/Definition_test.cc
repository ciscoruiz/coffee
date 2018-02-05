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

#include <boost/test/unit_test.hpp>

#include <functional>
#include <mutex>
#include <thread>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TraceMethod.hpp>
#include <coffee/logger/TtyWriter.hpp>

#include "MockApplication.hpp"
#include "MockDatabase.hpp"
#include "MockLowLevelRecord.hpp"

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
   MyDatabase(app::Application& app) : mock::MockDatabase(app) {;}
   MyDatabase(const char* name) : mock::MockDatabase(name) {;}

private:
   mock::MockLowLevelContainer m_container;

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

   if(record.m_time == 0)
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
      result.initialize(MyDatabase::LostConnection, NULL);
      return result;
   }

   mock::MockConnection::OpCode opCode =(getExpression() == "write") ? mock::MockConnection::Write: mock::MockConnection::Delete;

   MockLowLevelRecord record;

   record.m_id = coffee_datatype_downcast(dbms::datatype::Integer, m_datas[0])->getValue();

   if(record.m_id == test_dbms::IdToThrowDbException) {
      result.initialize(MyDatabase::NotFound, NULL);
      return result;
   }

   if(opCode != mock::MockConnection::Delete) {
      record.m_name = coffee_datatype_downcast(dbms::datatype::String, m_datas[1])->getValue();

      auto integer = coffee_datatype_downcast(dbms::datatype::Integer, m_datas[2]);
      record.m_integer =(integer->hasValue()) ? integer->getValue(): -1;

      record.m_float = coffee_datatype_downcast(dbms::datatype::Float, m_datas[3])->getValue();

      auto time = coffee_datatype_downcast(dbms::datatype::Date, m_datas[4]);
      record.m_time = (time->hasValue()) ? time->getValue(): 0;
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
   MockApplication application("dbms_define_structure");

   test_dbms::MyDatabase database(application);

   auto connection = database.createConnection("0", "0", "0");

   BOOST_REQUIRE_THROW(database.createConnection("0", "bis0", "bis0"), adt::RuntimeException);

   auto findConnection = database.findConnection("0");

   BOOST_REQUIRE_EQUAL(connection.get(), findConnection.get());

   BOOST_REQUIRE_THROW(database.findConnection("zzzz"), adt::RuntimeException);

   auto statement = database.createStatement("one", "write");

   BOOST_REQUIRE_THROW(database.createStatement("one", "write"), adt::RuntimeException);
   BOOST_REQUIRE_THROW(database.createStatement("the_null", "null"), adt::RuntimeException);

   auto findStatement = database.findStatement("one");

   BOOST_REQUIRE_EQUAL(statement.get(), findStatement.get());

   BOOST_REQUIRE_THROW(database.findStatement("zzzz"), adt::RuntimeException);

   adt::StreamString xxx = connection->operator coffee::adt::StreamString();

   BOOST_REQUIRE_NE(xxx.find("CommitCounter"), std::string::npos);
}

BOOST_AUTO_TEST_CASE(dbms_translator)
{
   MockApplication application("dbms_define_structure");

   test_dbms::MyDatabase database(application);

   auto st0 = database.createStatement("zero", "read");
   BOOST_REQUIRE_EQUAL(st0->getExpression(), "read");

   database.setStatementTranslator(std::make_shared<test_dbms::MyTranslator>());
   auto st1 = database.createStatement("one", "read");
   BOOST_REQUIRE_EQUAL(st1->getExpression(), "READ");
}

BOOST_AUTO_TEST_CASE(dbms_write_and_read_and_delete)
{
   test_dbms::MyDatabase database("dbms_write_and_read_and_delete");

   database.externalInitialize();

   std::shared_ptr<mock::MockConnection> conn0 = std::dynamic_pointer_cast<mock::MockConnection>(database.createConnection("0", "0", "0"));
   auto stWriter = database.createStatement("the_write", "write");
   auto stReader = database.createStatement("the_read", "read");
   auto stEraser = database.createStatement("the_erase", "delete");
   ResultCode resultCode;

   BOOST_REQUIRE_EQUAL(database.isRunning(), true);

   try {
      dbms::GuardConnection guard(conn0);
      dbms::GuardStatement writer(guard, stWriter);

      BOOST_REQUIRE_EQUAL(guard.getCountLinkedStatement(), 1);

      BOOST_REQUIRE_THROW(writer.getInputData(10), adt::RuntimeException);

      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(2);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the 2");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(2 * 2);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(2.2);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue("2/2/2002T02:02:02", "%d/%m/%YT%H:%M");
      writer.execute();

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 1);
      BOOST_REQUIRE_EQUAL(database.container_size(), 0);

      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(5);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the 5");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(5 * 5);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(5.5);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue("5/5/2005T05:05:05", "%d/%m/%YT%H:%M");
      writer.execute();

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 2);
      BOOST_REQUIRE_EQUAL(database.container_size(), 0);

      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(6);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the 6");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(6 * 6);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(6.6);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue("6/6/2006T06:06:06", "%d/%m/%YT%H:%M");
      resultCode = writer.execute();

      BOOST_REQUIRE_EQUAL(resultCode.getNumericCode(), test_dbms::MyDatabase::Successful);
      BOOST_REQUIRE_EQUAL(resultCode.successful(), true);
      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 3);
      BOOST_REQUIRE_EQUAL(database.container_size(), 0);
   }
   catch(adt::Exception& ex) {
      logger::Logger::write(ex);
      BOOST_REQUIRE_EQUAL(true, false);
   }

   BOOST_REQUIRE_EQUAL(conn0->operation_size(), 0);
   BOOST_REQUIRE_EQUAL(database.container_size(), 3);
   BOOST_REQUIRE_EQUAL(conn0->getCommitCounter(), 1);

   if(true) {
      dbms::GuardConnection guard(conn0);

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

   BOOST_REQUIRE_EQUAL(database.container_size(), 3);
   BOOST_REQUIRE_EQUAL(conn0->getCommitCounter(), 1); // There was not commit due to reader operation

   if(true) {
      dbms::GuardConnection guard(conn0);

      dbms::GuardStatement eraser(guard, stEraser);

      auto id = coffee_datatype_downcast(datatype::Integer, eraser.getInputData(0));

      id->setValue(6);
      resultCode = eraser.execute();
      BOOST_REQUIRE_EQUAL(resultCode.successful(), true);

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 1);
      BOOST_REQUIRE_EQUAL(database.container_size(), 3);

      id->setValue(2);
      resultCode = eraser.execute();
      BOOST_REQUIRE_EQUAL(resultCode.successful(), true);

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 2);
      BOOST_REQUIRE_EQUAL(database.container_size(), 3);
   }

   BOOST_REQUIRE_EQUAL(database.container_size(), 1);
   BOOST_REQUIRE_EQUAL(conn0->getCommitCounter(), 2);
}

BOOST_AUTO_TEST_CASE(dbms_write_rollback)
{
   test_dbms::MyDatabase database("dbms_write_rollback");

   database.externalInitialize();

   auto conn0 = std::dynamic_pointer_cast<mock::MockConnection>(database.createConnection("0", "0", "0"));
   auto rollbackWriter = database.createStatement("rollback_write", "write");
   ResultCode resultCode;

   BOOST_REQUIRE_EQUAL(database.isRunning(), true);

   if(true) {
      dbms::GuardConnection guard(conn0);
      dbms::GuardStatement writer(guard, rollbackWriter);

      BOOST_REQUIRE_EQUAL(guard.getCountLinkedStatement(), 1);

      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(2);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the 2");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(2 * 2);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(2.2);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue("2/2/2002T02:02:02", "%d/%m/%YT%H:%M");
      writer.execute();

      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(5);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the 5");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(5 * 5);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(5.5);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue("5/5/2005T05:05:05", "%d/%m/%YT%H:%M");
      writer.execute();

      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(6);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the 6");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(6 * 6);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(6.6);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue("6/6/2006T06:06:06", "%d/%m/%YT%H:%M");
      writer.execute();

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 3);
      BOOST_REQUIRE_EQUAL(database.container_size(), 0);
   }

   BOOST_REQUIRE_EQUAL(conn0->getCommitCounter(), 1);
   BOOST_REQUIRE_EQUAL(conn0->getRollbackCounter(), 0);

   if(true) {
      dbms::GuardConnection guard(conn0);
      dbms::GuardStatement writer(guard, rollbackWriter);

      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(8);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the 8");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(8 * 8);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(8.8);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue("8/8/2008T08:08:08", "%d/%m/%YT%H:%M");
      writer.execute();

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 1);
      BOOST_REQUIRE_EQUAL(database.container_size(), 3);

      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(test_dbms::IdToThrowDbException);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the test_dbms::IdToThrowDbException");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(test_dbms::IdToThrowDbException * test_dbms::IdToThrowDbException);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(3.3);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue("3/3/2003T03:03:03", "%d/%m/%YT%H:%M");

      BOOST_REQUIRE_THROW(writer.execute(), dbms::DatabaseException);
   }

   BOOST_REQUIRE_EQUAL(conn0->operation_size(), 0);
   BOOST_REQUIRE_EQUAL(database.container_size(), 3);
   BOOST_REQUIRE_EQUAL(conn0->getCommitCounter(), 1);
   BOOST_REQUIRE_EQUAL(conn0->getRollbackCounter(), 1);
}

BOOST_AUTO_TEST_CASE(dbms_write_norollback)
{
   test_dbms::MyDatabase database("dbms_write_norollback");

   database.externalInitialize();

   auto conn0 = std::dynamic_pointer_cast<mock::MockConnection>(database.createConnection("0", "0", "0"));
   auto noRollbackWriter = database.createStatement("no_rollback_write", "write", dbms::ActionOnError::Ignore);
   ResultCode resultCode;

   BOOST_REQUIRE_EQUAL(database.isRunning(), true);

   if(true) {
      dbms::GuardConnection guard(conn0);
      dbms::GuardStatement writer(guard, noRollbackWriter);

      BOOST_REQUIRE_EQUAL(guard.getCountLinkedStatement(), 1);

      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(2);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the 2");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(2 * 2);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(2.2);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue("2/2/2002T02:02:02", "%d/%m/%YT%H:%M");
      writer.execute();

      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(5);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the 5");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(5 * 5);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(5.5);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue("5/5/2005T05:05:05", "%d/%m/%YT%H:%M");
      writer.execute();

      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(6);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the 6");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(6 * 6);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(6.6);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue("6/6/2006T06:06:06", "%d/%m/%YT%H:%M");
      writer.execute();

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 3);
      BOOST_REQUIRE_EQUAL(database.container_size(), 0);
   }

   BOOST_REQUIRE_EQUAL(conn0->getCommitCounter(), 1);
   BOOST_REQUIRE_EQUAL(conn0->getRollbackCounter(), 0);


   BOOST_REQUIRE_EQUAL(conn0->operation_size(), 0);
   BOOST_REQUIRE_EQUAL(database.container_size(), 3);
   BOOST_REQUIRE_EQUAL(conn0->getCommitCounter(), 1);
   BOOST_REQUIRE_EQUAL(conn0->getRollbackCounter(), 0);

   if(true) {
      dbms::GuardConnection guard(conn0);
      dbms::GuardStatement writer(guard, noRollbackWriter);

      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(8);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the 8");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(8 * 8);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(8.8);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue("8/8/2008T08:08:08", "%d/%m/%YT%H:%M");
      writer.execute();

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 1);
      BOOST_REQUIRE_EQUAL(database.container_size(), 3);

      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(test_dbms::IdToThrowDbException);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the test_dbms::IdToThrowDbException");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(test_dbms::IdToThrowDbException * test_dbms::IdToThrowDbException);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(3.3);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue("3/3/2003T03:03:03", "%d/%m/%YT%H:%M");

      BOOST_REQUIRE_NO_THROW(resultCode = writer.execute());

      BOOST_REQUIRE_EQUAL(resultCode.successful(), false);
      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 1);
      BOOST_REQUIRE_EQUAL(database.container_size(), 3);
   }

   BOOST_REQUIRE_EQUAL(conn0->operation_size(), 0);
   BOOST_REQUIRE_EQUAL(database.container_size(), 4);
   BOOST_REQUIRE_EQUAL(conn0->getCommitCounter(), 2);
   BOOST_REQUIRE_EQUAL(conn0->getRollbackCounter(), 0);
}

BOOST_AUTO_TEST_CASE(dbms_erase_rollback)
{
   test_dbms::MyDatabase database("dbms_erase_rollback");

   database.externalInitialize();

   auto conn0 = std::dynamic_pointer_cast<mock::MockConnection>(database.createConnection("0", "0", "0"));
   auto stWriter = database.createStatement("writer", "write");
   auto rollbackEraser = database.createStatement("rollback_eraser", "delete");
   auto noRollbackEraser = database.createStatement("no_rollback_eraser", "delete", dbms::ActionOnError::Ignore);
   ResultCode resultCode;

   BOOST_REQUIRE_EQUAL(database.isRunning(), true);

   if(true) {
      dbms::GuardConnection guard(conn0);
      dbms::GuardStatement writer(guard, stWriter);

      BOOST_REQUIRE_EQUAL(guard.getCountLinkedStatement(), 1);

      BOOST_REQUIRE_THROW(writer.getInputData(10), adt::RuntimeException);

      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(2);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the 2");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(2 * 2);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(2.2);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue("2/2/2002T02:02:02", "%d/%m/%YT%H:%M");
      writer.execute();


      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(5);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the 5");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(5 * 5);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(5.5);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue("5/5/2005T05:05:05", "%d/%m/%YT%H:%M");
      writer.execute();

      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(6);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the 6");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(6 * 6);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(6.6);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue("6/6/2006T06:06:06", "%d/%m/%YT%H:%M");
      writer.execute();

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 3);
      BOOST_REQUIRE_EQUAL(database.container_size(), 0);
   }

   BOOST_REQUIRE_EQUAL(conn0->operation_size(), 0);
   BOOST_REQUIRE_EQUAL(database.container_size(), 3);
   BOOST_REQUIRE_EQUAL(conn0->getCommitCounter(), 1);
   BOOST_REQUIRE_EQUAL(conn0->getRollbackCounter(), 0);

   if(true) {
      dbms::GuardConnection guard(conn0);
      dbms::GuardStatement eraser(guard, rollbackEraser);

      coffee_datatype_downcast(datatype::Integer, eraser.getInputData(0))->setValue(6);
      eraser.execute();

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 1);
      BOOST_REQUIRE_EQUAL(database.container_size(), 3);

      coffee_datatype_downcast(datatype::Integer, eraser.getInputData(0))->setValue(test_dbms::IdToThrowDbException);
      BOOST_REQUIRE_THROW(eraser.execute(), dbms::DatabaseException);

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 1);
      BOOST_REQUIRE_EQUAL(database.container_size(), 3);
   }

   BOOST_REQUIRE_EQUAL(conn0->operation_size(), 0);
   BOOST_REQUIRE_EQUAL(database.container_size(), 3);
   BOOST_REQUIRE_EQUAL(conn0->getCommitCounter(), 1);
   BOOST_REQUIRE_EQUAL(conn0->getRollbackCounter(), 1);
}

BOOST_AUTO_TEST_CASE(dbms_erase_norollback)
{
   test_dbms::MyDatabase database("dbms_erase_norollback");

   database.externalInitialize();

   auto conn0 = std::dynamic_pointer_cast<mock::MockConnection>(database.createConnection("0", "0", "0"));
   auto stWriter = database.createStatement("writer", "write");
   auto noRollbackEraser = database.createStatement("no_rollback_eraser", "delete", dbms::ActionOnError::Ignore);
   ResultCode resultCode;

   BOOST_REQUIRE_EQUAL(database.isRunning(), true);

   if(true) {
      dbms::GuardConnection guard(conn0);
      dbms::GuardStatement writer(guard, stWriter);

      BOOST_REQUIRE_EQUAL(guard.getCountLinkedStatement(), 1);

      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(2);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the 2");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(2 * 2);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(2.2);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue("2/2/2002T02:02:02", "%d/%m/%YT%H:%M");
      writer.execute();


      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(5);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the 5");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(5 * 5);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(5.5);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue("5/5/2005T05:05:05", "%d/%m/%YT%H:%M");
      writer.execute();

      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(6);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the 6");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(6 * 6);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(6.6);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue("6/6/2006T06:06:06", "%d/%m/%YT%H:%M");
      writer.execute();

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 3);
      BOOST_REQUIRE_EQUAL(database.container_size(), 0);
   }

   BOOST_REQUIRE_EQUAL(conn0->operation_size(), 0);
   BOOST_REQUIRE_EQUAL(database.container_size(), 3);
   BOOST_REQUIRE_EQUAL(conn0->getCommitCounter(), 1);
   BOOST_REQUIRE_EQUAL(conn0->getRollbackCounter(), 0);

   if(true) {
      dbms::GuardConnection guard(conn0);
      dbms::GuardStatement eraser(guard, noRollbackEraser);

      coffee_datatype_downcast(datatype::Integer, eraser.getInputData(0))->setValue(2);
      eraser.execute();

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 1);
      BOOST_REQUIRE_EQUAL(database.container_size(), 3);

      coffee_datatype_downcast(datatype::Integer, eraser.getInputData(0))->setValue(test_dbms::IdToThrowDbException);
      BOOST_REQUIRE_NO_THROW(resultCode = eraser.execute());

      BOOST_REQUIRE_EQUAL(resultCode.successful(), false);
      BOOST_REQUIRE_EQUAL(resultCode.notFound(), true);
      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 1);
      BOOST_REQUIRE_EQUAL(database.container_size(), 3);
   }

   BOOST_REQUIRE_EQUAL(conn0->operation_size(), 0);
   BOOST_REQUIRE_EQUAL(database.container_size(), 2);
   BOOST_REQUIRE_EQUAL(conn0->getCommitCounter(), 2);
   BOOST_REQUIRE_EQUAL(conn0->getRollbackCounter(), 0);
}

BOOST_AUTO_TEST_CASE(dbms_set_max_commit)
{
   test_dbms::MyDatabase database("dbms_set_max_commit");

   database.externalInitialize();

   auto conn0 = std::dynamic_pointer_cast<mock::MockConnection>(database.createConnection("0", "0", "0"));
   auto stWriter = database.createStatement("the_write", "write");
   auto stReader = database.createStatement("the_read", "read");

   BOOST_REQUIRE_EQUAL(database.isRunning(), true);

   if(true) {
      GuardConnection connection(conn0);
      GuardStatement writer(connection, stWriter);

      BOOST_REQUIRE_EQUAL(connection.setMaxCommitPending(5), 0);

      for(int ii = 1; ii <= 15; ++ ii) {
         coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(ii);
         coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the ii");
         coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(ii * ii);
         coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(100 / ii);
         coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue(adt::Second(ii));
         BOOST_REQUIRE_NO_THROW(writer.execute());
      }

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 0);
      BOOST_REQUIRE_EQUAL(conn0->getCommitCounter(), 3);
      BOOST_REQUIRE_EQUAL(conn0->getRollbackCounter(), 0);
   }

   BOOST_REQUIRE_EQUAL(database.container_size(), 15);

   if(true) {
      GuardConnection connection(conn0);
      GuardStatement writer(connection, stWriter);

      connection.clearMaxCommitPending();

      for(int ii = 16; ii <= 30; ++ ii) {
         coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(ii);
         coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the ii");
         coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(ii * ii);
         coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(100 / ii);
         coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue(adt::Second(ii));
         BOOST_REQUIRE_NO_THROW(writer.execute());
      }
   }

   BOOST_REQUIRE_EQUAL(conn0->getCommitCounter(), 4);
   BOOST_REQUIRE_EQUAL(conn0->getRollbackCounter(), 0);
   BOOST_REQUIRE_EQUAL(conn0->operation_size(), 0);
   BOOST_REQUIRE_EQUAL(database.container_size(), 30);
}

BOOST_AUTO_TEST_CASE(dbms_break_detected_executing)
{
   test_dbms::MyDatabase database("dbms_break_detected_executing");

   database.externalInitialize();

   auto conn0 = std::dynamic_pointer_cast<mock::MockConnection>(database.createConnection("0", "0", "0"));
   auto stWriter = database.createStatement("the_write", "write");

   BOOST_REQUIRE_EQUAL(database.isRunning(), true);

   if(true) {
      GuardConnection connection(conn0);
      GuardStatement writer(connection, stWriter);

      BOOST_REQUIRE_EQUAL(conn0->getOpenCounter(), 1);
      BOOST_REQUIRE_EQUAL(conn0->getCloseCounter(), 0);

      int ii = 1;
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(ii);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the ii");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(ii * ii);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(100 / ii);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue(adt::Second(ii));
      BOOST_REQUIRE_NO_THROW(writer.execute());

      conn0->manualBreak();

      ii = 2;
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(ii);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the ii");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(ii * ii);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(100 / ii);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue(adt::Second(ii));

      // It will detect the lost connection but it will try to recover and it will get it.
      BOOST_REQUIRE_THROW(writer.execute(), dbms::DatabaseException);

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 0);
      BOOST_REQUIRE_EQUAL(conn0->getOpenCounter(), 2);
      BOOST_REQUIRE_EQUAL(conn0->getCloseCounter(), 1);
   }

   // It will recover the connection after it will detect
   BOOST_REQUIRE_EQUAL(conn0->isAvailable(), true);
   BOOST_REQUIRE_EQUAL(database.container_size(), 0);
}

BOOST_AUTO_TEST_CASE(dbms_break_detected_locking)
{
   test_dbms::MyDatabase database("dbms_break_detected_locking");

   database.externalInitialize();

   auto conn0 = std::dynamic_pointer_cast<mock::MockConnection>(database.createConnection("0", "0", "0"));
   auto stWriter = database.createStatement("the_write", "write");

   BOOST_REQUIRE_EQUAL(database.isRunning(), true);

   if(true) {
      GuardConnection connection(conn0);
      GuardStatement writer(connection, stWriter);

      BOOST_REQUIRE_EQUAL(conn0->getOpenCounter(), 1);
      BOOST_REQUIRE_EQUAL(conn0->getCloseCounter(), 0);

      int ii = 1;
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(ii);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the ii");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(ii * ii);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(100 / ii);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue(adt::Second(ii));
      BOOST_REQUIRE_NO_THROW(writer.execute());
   }

   conn0->manualBreak();

   if(true) {
      // It will detect the lost connection but it will try to recover and it will get it.
      GuardConnection connection(conn0);
      GuardStatement writer(connection, stWriter);

      BOOST_REQUIRE_EQUAL(conn0->getOpenCounter(), 2);
      BOOST_REQUIRE_EQUAL(conn0->getCloseCounter(), 1);

      int ii = 2;
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(ii);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the ii");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(ii * ii);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(100 / ii);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue(adt::Second(ii));

      BOOST_REQUIRE_NO_THROW(writer.execute());
   }

   // It will recover the connection after it will detect
   BOOST_REQUIRE_EQUAL(conn0->isAvailable(), true);
   BOOST_REQUIRE_EQUAL(database.container_size(), 2);

   if(true) {
      // It will detect the lost connection but it will try to recover and it will get it.
      GuardConnection connection(conn0);

      conn0->manualBreak();

      BOOST_REQUIRE_THROW(GuardStatement writer(connection, stWriter), adt::RuntimeException);

      BOOST_REQUIRE_EQUAL(conn0->getOpenCounter(), 2);
      BOOST_REQUIRE_EQUAL(conn0->getCloseCounter(), 1);
   }

   if(true) {
      GuardConnection connection(conn0);
      GuardStatement writer(connection, stWriter);

      BOOST_REQUIRE_EQUAL(conn0->getOpenCounter(), 3);
      BOOST_REQUIRE_EQUAL(conn0->getCloseCounter(), 2);

      int ii = 3;
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(ii);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the ii");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(ii * ii);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(100 / ii);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue(adt::Second(ii));

      BOOST_REQUIRE_NO_THROW(writer.execute());
   }

   // It will recover the connection after it will detect
   BOOST_REQUIRE_EQUAL(conn0->isAvailable(), true);
   BOOST_REQUIRE_EQUAL(database.container_size(), 3);
}

BOOST_AUTO_TEST_CASE(dbms_break_unrecovery_executing)
{
   test_dbms::MyDatabase database("dbms_break_unrecovery_executing");

   database.externalInitialize();

   std::shared_ptr<test_dbms::MyRecoveryHandler> recoveryHandler = std::make_shared<test_dbms::MyRecoveryHandler>();
   database.setFailRecoveryHandler(recoveryHandler);

   auto conn0 = std::dynamic_pointer_cast<mock::MockConnection>(database.createConnection("0", "0", "0"));
   auto stWriter = database.createStatement("the_write", "write");

   BOOST_REQUIRE_EQUAL(database.isRunning(), true);

   if(true) {
      GuardConnection connection(conn0);
      GuardStatement writer(connection, stWriter);

      BOOST_REQUIRE_EQUAL(conn0->getOpenCounter(), 1);
      BOOST_REQUIRE_EQUAL(conn0->getCloseCounter(), 0);

      int ii = 1;
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(ii);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the ii");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(ii * ii);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(100 / ii);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue(adt::Second(ii));
      BOOST_REQUIRE_NO_THROW(writer.execute());

      conn0->manualBreak();
      conn0->avoidRecovery();

      ii = 2;
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(ii);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the ii");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(ii * ii);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(100 / ii);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue(adt::Second(ii));

      // It will detect the lost connection but it will try to recover and it will get it.
      BOOST_REQUIRE_THROW(writer.execute(), dbms::DatabaseException);

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 0);
      BOOST_REQUIRE_EQUAL(conn0->getOpenCounter(), 1);
      BOOST_REQUIRE_EQUAL(conn0->getCloseCounter(), 1);
      BOOST_REQUIRE_EQUAL(recoveryHandler->thisWasUsed(), 1);
   }

   // It will recover the connection after it will detect
   BOOST_REQUIRE_EQUAL(conn0->isAvailable(), false);
   BOOST_REQUIRE_EQUAL(database.container_size(), 0);
}

BOOST_AUTO_TEST_CASE(dbms_break_unrecovery_locking)
{
   test_dbms::MyDatabase database("dbms_break_unrecovery_locking");

   database.externalInitialize();

   std::shared_ptr<test_dbms::MyRecoveryHandler> recoveryHandler = std::make_shared<test_dbms::MyRecoveryHandler>();
   database.setFailRecoveryHandler(recoveryHandler);

   auto conn0 = std::dynamic_pointer_cast<mock::MockConnection>(database.createConnection("0", "0", "0"));
   auto stWriter = database.createStatement("the_write", "write");

   BOOST_REQUIRE_EQUAL(database.isRunning(), true);

   if(true) {
      GuardConnection connection(conn0);
      GuardStatement writer(connection, stWriter);

      BOOST_REQUIRE_EQUAL(conn0->getOpenCounter(), 1);
      BOOST_REQUIRE_EQUAL(conn0->getCloseCounter(), 0);

      int ii = 1;
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(ii);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the ii");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(ii * ii);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(100 / ii);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue(adt::Second(ii));
      BOOST_REQUIRE_NO_THROW(writer.execute());
   }

   conn0->manualBreak();
   conn0->avoidRecovery();

   if(true) {
      BOOST_REQUIRE_THROW(GuardConnection connection(conn0), adt::RuntimeException);
      BOOST_REQUIRE_EQUAL(conn0->getOpenCounter(), 1);
      BOOST_REQUIRE_EQUAL(conn0->getCloseCounter(), 1);
      BOOST_REQUIRE_EQUAL(recoveryHandler->thisWasUsed(), 1);
   }

   // It will recover the connection after it will detect
   BOOST_REQUIRE_EQUAL(conn0->isAvailable(), false);
   BOOST_REQUIRE_EQUAL(database.container_size(), 1);
}

BOOST_AUTO_TEST_CASE(dbms_dealing_with_nulls)
{
   test_dbms::MyDatabase database("dbms_dealing_with_nulls");

   database.externalInitialize();

   auto conn0 = std::dynamic_pointer_cast<mock::MockConnection>(database.createConnection("0", "0", "0"));
   auto stWriter = database.createStatement("the_write", "write");
   auto stReader = database.createStatement("the_read", "read");
   ResultCode resultCode;

   BOOST_REQUIRE_EQUAL(database.isRunning(), true);

   if(true) {
      try {
         dbms::GuardConnection guard(conn0);
         dbms::GuardStatement writer(guard, stWriter);

         coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(20);
         coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the 20");
         coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->isNull();
         coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(20.20);
         coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue("2/2/2002T02:02:02", "%d/%m/%YT%H:%M");
         writer.execute();

         BOOST_REQUIRE_EQUAL(conn0->operation_size(), 1);
         BOOST_REQUIRE_EQUAL(database.container_size(), 0);

         coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(25);
         coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the 25");
         coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(25 * 25);
         coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(25.25);
         coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->isNull();
         ResultCode resultCode = writer.execute();

         BOOST_REQUIRE_EQUAL(conn0->operation_size(), 2);
         BOOST_REQUIRE_EQUAL(database.container_size(), 0);

         BOOST_REQUIRE_EQUAL(resultCode.getNumericCode(), test_dbms::MyDatabase::Successful);
         BOOST_REQUIRE_EQUAL(resultCode.successful(), true);
         BOOST_REQUIRE_EQUAL(conn0->operation_size(), 2);
         BOOST_REQUIRE_EQUAL(database.container_size(), 0);
      }
      catch(adt::Exception& ex) {
         logger::Logger::write(ex);
      }
   }

   BOOST_REQUIRE_EQUAL(conn0->operation_size(), 0);
   BOOST_REQUIRE_EQUAL(database.container_size(), 2);
   BOOST_REQUIRE_EQUAL(conn0->getCommitCounter(), 1);

   if(true) {
      dbms::GuardConnection guard(conn0);

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

BOOST_AUTO_TEST_CASE(dbms_without_app)
{
   test_dbms::MyDatabase database("dbms_without_app");

   BOOST_REQUIRE_NO_THROW(database.externalInitialize());

   BOOST_REQUIRE_EQUAL(database.isRunning(), true);

   auto conn0 = std::dynamic_pointer_cast<mock::MockConnection>(database.createConnection("0", "0", "0"));
   auto stWriter = database.createStatement("the_write", "write");
   auto stReader = database.createStatement("the_read", "read");
   ResultCode resultCode;

   try {
      dbms::GuardConnection guard(conn0);
      dbms::GuardStatement writer(guard, stWriter);

      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(20);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the 20");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->isNull();
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(20.20);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->setValue("2/2/2002T02:02:02", "%d/%m/%YT%H:%M");
      writer.execute();

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 1);
      BOOST_REQUIRE_EQUAL(database.container_size(), 0);

      coffee_datatype_downcast(datatype::Integer, writer.getInputData(0))->setValue(25);
      coffee_datatype_downcast(datatype::String, writer.getInputData(1))->setValue("the 25");
      coffee_datatype_downcast(datatype::Integer, writer.getInputData(2))->setValue(25 * 25);
      coffee_datatype_downcast(datatype::Float, writer.getInputData(3))->setValue(25.25);
      coffee_datatype_downcast(datatype::Date, writer.getInputData(4))->isNull();
      ResultCode resultCode = writer.execute();

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 2);
      BOOST_REQUIRE_EQUAL(database.container_size(), 0);

      BOOST_REQUIRE_EQUAL(resultCode.getNumericCode(), test_dbms::MyDatabase::Successful);
      BOOST_REQUIRE_EQUAL(resultCode.successful(), true);
      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 2);
      BOOST_REQUIRE_EQUAL(database.container_size(), 0);
   }
   catch(adt::Exception& ex) {
      logger::Logger::write(ex);
      BOOST_REQUIRE_EQUAL(std::string("none"), ex.what());
   }

   BOOST_REQUIRE_EQUAL(conn0->operation_size(), 0);
   BOOST_REQUIRE_EQUAL(database.container_size(), 2);
   BOOST_REQUIRE_EQUAL(conn0->getCommitCounter(), 1);

   BOOST_REQUIRE_NO_THROW(database.externalStop());
   BOOST_REQUIRE_EQUAL(database.isRunning(), false);
}

/*
BOOST_AUTO_TEST_CASE(dbms_link_guards)
{
   MockApplication application("dbms_link_guards");

   std::shared_ptr<test_dbms::MyDatabase> database = std::make_shared<test_dbms::MyDatabase>(application);

   application.attach(database);

   std::shared_ptr<dbms::Connection> conn0;
   std::shared_ptr<dbms::Statement> st0;
   std::shared_ptr<dbms::Statement> st1;

   BOOST_REQUIRE_NO_THROW(conn0 = database->createConnection("0", "0", "0"));
   BOOST_REQUIRE_NO_THROW(st0 = database->createStatement("zero", "write"));
   BOOST_REQUIRE_NO_THROW(st1 = database->createStatement("one", "write"));

   std::shared_ptr<std::thread> tr;

   {
      std::lock_guard<std::mutex> guard(application.m_termination);

      tr = std::make_shared<std::thread>(std::ref(application));
      usleep(400);

      BOOST_REQUIRE_EQUAL(application.isRunning(), true);
      BOOST_REQUIRE_EQUAL(database->isRunning(), true);

      if(true) {
         dbms::GuardConnection guardConnection(conn0);

         BOOST_REQUIRE_EQUAL(guardConnection.getCountLinkedStatement(), 0);

         if(true) {
            dbms::GuardStatement guardSt0(guardConnection, st0);

            BOOST_REQUIRE_EQUAL(guardConnection.getCountLinkedStatement(), 1);

            if(true) {
               dbms::GuardStatement guardSt1(guardConnection, st1);
               BOOST_REQUIRE_EQUAL(guardConnection.getCountLinkedStatement(), 2);
            }

            BOOST_REQUIRE_EQUAL(guardConnection.getCountLinkedStatement(), 1);
         }

         BOOST_REQUIRE_EQUAL(guardConnection.getCountLinkedStatement(), 0);
      }

      LOG_DEBUG("Enables termination");
   }

   tr->join();
}

*/
