// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2013 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/wepa
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//

#include <boost/test/unit_test.hpp>

#include <functional>
#include <mutex>
#include <thread>

#include <wepa/logger/Logger.hpp>
#include <wepa/logger/TraceMethod.hpp>
#include <wepa/logger/TtyWriter.hpp>

#include <mock/MockApplication.hpp>
#include <mock/MockLowLevelRecord.hpp>
#include <mock/MockDatabase.hpp>

#include <wepa/dbms/Statement.hpp>
#include <wepa/dbms/GuardConnection.hpp>
#include <wepa/dbms/GuardStatement.hpp>
#include <wepa/dbms/StatementTranslator.hpp>
#include <wepa/dbms/FailRecoveryHandler.hpp>

#include <wepa/dbms/datatype/Integer.hpp>
#include <wepa/dbms/datatype/String.hpp>
#include <wepa/dbms/datatype/Float.hpp>
#include <wepa/dbms/datatype/Date.hpp>

namespace wepa {

namespace test_dbms {

using namespace wepa::dbms;

class MyStatement : public Statement {
protected:
   MyStatement (Database& database, const char* name, const char* expression, const ActionOnError::_v actionOnError) :
      Statement (database, name, expression, actionOnError),
      m_id ("ID"),
      m_name ("name", 64),
      m_integer ("integer", datatype::Constraint::CanBeNull),
      m_float ("float"),
      m_time ("time", datatype::Constraint::CanBeNull)
   {;}

   mock::MockLowLevelRecord& getRecord () noexcept { return std::ref (m_record); }

protected:
   binder::BinderIf* m_binders [5];

   datatype::Integer m_id;
   datatype::String m_name;
   datatype::Integer m_integer;
   datatype::Float m_float;
   datatype::Date m_time;

private:
   mock::MockLowLevelRecord m_record;
};

class MyReadStatement : public MyStatement {
public:
   MyReadStatement (Database& database, const char* name, const char* expression, const ActionOnError::_v actionOnError);

private:
   std::vector <mock::MockLowLevelRecord> m_selection;
   int m_index;

   void do_prepare (Connection* connection) throw (adt::RuntimeException, DatabaseException) {;}
   ResultCode do_execute (Connection& connection) throw (adt::RuntimeException, DatabaseException);
   bool do_fetch () throw (adt::RuntimeException, DatabaseException);
};

class MyWriteStatement : public MyStatement {
public:
   MyWriteStatement (Database& database, const char* name, const char* expression, const ActionOnError::_v actionOnError);

private:
   void do_prepare (Connection* connection) throw (adt::RuntimeException, DatabaseException) {;}
   ResultCode do_execute (Connection& connection) throw (adt::RuntimeException, DatabaseException);
   bool do_fetch () throw (adt::RuntimeException, DatabaseException) { return false; }
};

class MyDatabase : public mock::MockDatabase {
public:
   MyDatabase (app::Application& app) : mock::MockDatabase (app) {;}

private:
   mock::MockLowLevelContainer m_container;

   Statement* allocateStatement (const char* name, const std::string& expression, const ActionOnError::_v actionOnError)
      throw (adt::RuntimeException)
   {
      if (expression == "read" || expression == "READ")
         return new MyReadStatement (std::ref (*this), name, expression.c_str(), actionOnError);

      if (expression == "write" || expression == "delete")
         return new MyWriteStatement (std::ref (*this), name, expression.c_str(), actionOnError);

      return NULL;
   }
};

class MyTranslator : public dbms::StatementTranslator {
public:
   MyTranslator () : dbms::StatementTranslator ("MyTranslator"), m_buffer (NULL) {;}
   ~MyTranslator () { if (m_buffer != NULL) free (m_buffer); }

private:
   char* m_buffer;

   const char* apply (const char* statement) throw (adt::RuntimeException);
};

class MyRecoveryHandler : public dbms::FailRecoveryHandler {
public:
   MyRecoveryHandler () : m_thisWasUsed (0) {;}

   int thisWasUsed () const noexcept { return m_thisWasUsed; }

private:
   int m_thisWasUsed;

   void apply (const dbms::Connection& connection) throw (adt::RuntimeException) {
      ++ m_thisWasUsed;
   }
};

}
}

using namespace wepa;
using namespace wepa::dbms;
using namespace wepa::mock;

test_dbms::MyReadStatement::MyReadStatement (Database& database, const char* name, const char* expression, const ActionOnError::_v actionOnError) :
   MyStatement (database, name, expression, actionOnError),
   m_index (0)
{
   this->createBinderInput(m_id);
   this->createBinderOutput (m_name);
   this->createBinderOutput(m_integer);
   this->createBinderOutput(m_float);
   this->createBinderOutput(m_time);
}

dbms::ResultCode test_dbms::MyReadStatement::do_execute (dbms::Connection& connection)
   throw (adt::RuntimeException, DatabaseException)
{
   LOG_THIS_METHOD();

   m_index = 0;

   mock::MockConnection& _connection (static_cast <mock::MockConnection&> (connection));

   m_selection.clear ();
   for (auto ii : _connection.getContainer()) {
      if (ii.second.m_id >= m_id.getValue()) {
         m_selection.push_back(ii.second);
      }
   }

   LOG_DEBUG ("n-size:" << m_selection.size());

   return ResultCode (getDatabase(), (m_selection.size () == 0) ? MyDatabase::NotFound: MyDatabase::Successful);
}

bool test_dbms::MyReadStatement::do_fetch () throw (adt::RuntimeException, DatabaseException)
{
   LOG_THIS_METHOD();

   if (m_index >= m_selection.size ())
      return false;

   const MockLowLevelRecord& record = m_selection [m_index ++];

    m_id.setValue (record.m_id);
    m_name.setValue(record.m_name);
   
   if (record.m_integer == -1)
      m_integer.isNull ();
   else
      m_integer.setValue (record.m_integer);

   m_float.setValue (record.m_float);

   if (record.m_time == 0)
      m_time.isNull ();
   else
      m_time.setValue (record.m_time);

   LOG_DEBUG (m_id << " | " << m_integer  << " | " << m_time);

   return true;
}

test_dbms::MyWriteStatement::MyWriteStatement (Database& database, const char* name, const char* expression, const ActionOnError::_v actionOnError) :
   MyStatement (database, name, expression, actionOnError)
{
   this->createBinderInput(m_id);
   this->createBinderInput (m_name);
   this->createBinderInput(m_integer);
   this->createBinderInput(m_float);
   this->createBinderInput(m_time);
}

dbms::ResultCode test_dbms::MyWriteStatement::do_execute (dbms::Connection& connection)
   throw (adt::RuntimeException, DatabaseException)
{
   LOG_THIS_METHOD();

   dbms::ResultCode result (getDatabase(), MyDatabase::Successful);

   if (connection.isAvailable() == false) {
      LOG_DEBUG (connection << " | Connection is not available");
      result.initialize(MyDatabase::LostConnection, NULL);
      return result;
   }

   mock::MockConnection::OpCode opCode = (getExpression() == "write") ? mock::MockConnection::Write: mock::MockConnection::Delete;

   MockLowLevelRecord record;

   record.m_id = m_id.getValue ();

   if (record.m_id == 666) {
      result.initialize(MyDatabase::NotFound, NULL);
      return result;
   }

   if (opCode != mock::MockConnection::Delete) {
      record.m_name = m_name.getValue ();
      record.m_integer = (m_integer.hasValue ()) ? m_integer.getValue (): -1;
      record.m_float = m_float.getValue();
      record.m_time = (m_time.hasValue ()) ? m_time.getValue (): 0;
   }

   LOG_DEBUG ("ID = " << record.m_id);

   static_cast <mock::MockConnection&> (connection).addOperation(opCode, record);

   return result;
}

const char* test_dbms::MyTranslator::apply (const char* statement)
   throw (adt::RuntimeException)
{
   if (m_buffer != NULL) {
      free (m_buffer);
      m_buffer = NULL;
   }

   m_buffer = strdup (statement);
   char* ww = m_buffer;

   while (*ww) {
      *ww = toupper(*ww);
      ++ ww;
   }

   return m_buffer;
}

BOOST_AUTO_TEST_CASE (dbms_define_structure)
{
   MockApplication application ("dbms_define_structure");

   test_dbms::MyDatabase database (application);

   dbms::Connection* conn0 = database.createConnection("0", "0", "0");

   BOOST_REQUIRE_THROW (database.createConnection("0", "bis0", "bis0"), adt::RuntimeException);

   dbms::Connection& conn = database.findConnection("0");

   BOOST_REQUIRE_EQUAL (conn0, &conn);

   BOOST_REQUIRE_THROW (database.findConnection("zzzz"), adt::RuntimeException);

   dbms::Statement* st0 = database.createStatement("one", "write");

   BOOST_REQUIRE_THROW (database.createStatement("one", "write"), adt::RuntimeException);
   BOOST_REQUIRE_THROW (database.createStatement("the_null", "null"), adt::RuntimeException);

   dbms::Statement& st = database.findStatement("one");

   BOOST_REQUIRE_EQUAL (st0, &st);

   BOOST_REQUIRE_THROW (database.findStatement("zzzz"), adt::RuntimeException);

   adt::StreamString xxx = conn;

   BOOST_REQUIRE_NE (xxx.find ("CommitCounter"), std::string::npos);

}

BOOST_AUTO_TEST_CASE (dbms_translator)
{
   MockApplication application ("dbms_define_structure");

   test_dbms::MyDatabase database (application);
   test_dbms::MyTranslator translator;

   dbms::Statement* st0 = database.createStatement("zero", "read");
   BOOST_REQUIRE_EQUAL (st0->getExpression (), "read");

   database.setStatementTranslator(&translator);

   dbms::Statement* st1 = database.createStatement("one", "read");
   BOOST_REQUIRE_EQUAL (st1->getExpression (), "READ");
}

BOOST_AUTO_TEST_CASE (dbms_link_guards)
{
   MockApplication application ("dbms_link_guards");

   test_dbms::MyDatabase database (application);

   dbms::Connection* conn0;
   dbms::Statement* st0;
   dbms::Statement* st1;

   BOOST_REQUIRE_NO_THROW(conn0 = database.createConnection("0", "0", "0"));
   BOOST_REQUIRE_NO_THROW(st0 = database.createStatement("zero", "write"));
   BOOST_REQUIRE_NO_THROW(st1= database.createStatement("one", "write"));

   application.disableTermination();

   std::thread tr (std::ref (application));
   usleep (500);

   BOOST_REQUIRE_EQUAL (application.isRunning(), true);
   BOOST_REQUIRE_EQUAL (database.isRunning(), true);

   if (true) {
      dbms::GuardConnection guardConnection (conn0);

      BOOST_REQUIRE_EQUAL(guardConnection.getCountLinkedStatement(), 0);

      if (true) {
         dbms::GuardStatement guardSt0 (guardConnection, st0);

         BOOST_REQUIRE_EQUAL(guardConnection.getCountLinkedStatement(), 1);

         if (true) {
            dbms::GuardStatement guardSt1 (guardConnection, st1);
            BOOST_REQUIRE_EQUAL(guardConnection.getCountLinkedStatement(), 2);
         }

         BOOST_REQUIRE_EQUAL(guardConnection.getCountLinkedStatement(), 1);
      }

      BOOST_REQUIRE_EQUAL(guardConnection.getCountLinkedStatement(), 0);
   }

   LOG_DEBUG ("Enables termination");
   application.enableTermination();

   tr.join ();
}

BOOST_AUTO_TEST_CASE (dbms_write_and_read_and_delete)
{
   MockApplication application ("dbms_write_and_read_and_delete");

   test_dbms::MyDatabase database (application);

   application.disableTermination();

   mock::MockConnection* conn0 = static_cast <mock::MockConnection*> (database.createConnection("0", "0", "0"));
   dbms::Statement* stWriter = database.createStatement("the_write", "write");
   dbms::Statement* stReader = database.createStatement("the_read", "read");
   dbms::Statement* stEraser = database.createStatement("the_erase", "delete");
   ResultCode resultCode;

   std::thread tr (std::ref (application));
   usleep (500);

   BOOST_REQUIRE_EQUAL (application.isRunning(), true);
   BOOST_REQUIRE_EQUAL (database.isRunning(), true);

   if (true) {
      try {
         dbms::GuardConnection guard (conn0);
         dbms::GuardStatement writer (guard, stWriter);

         BOOST_REQUIRE_EQUAL (guard.getCountLinkedStatement(), 1);

         BOOST_REQUIRE_THROW(writer.getInputData(10), adt::RuntimeException);

         wepa_datatype_downcast(datatype::Integer, writer.getInputData(0)).setValue (2);
         wepa_datatype_downcast(datatype::String, writer.getInputData(1)).setValue ("the 2");
         wepa_datatype_downcast(datatype::Integer, writer.getInputData(2)).setValue (2 * 2);
         wepa_datatype_downcast(datatype::Float, writer.getInputData(3)).setValue (2.2);
         wepa_datatype_downcast(datatype::Date, writer.getInputData(4)).setValue ("2/2/2002T02:02:02", "%d/%m/%YT%H:%M");
         writer.execute ();

         BOOST_REQUIRE_EQUAL(conn0->operation_size(), 1);
         BOOST_REQUIRE_EQUAL (database.container_size(), 0);

         wepa_datatype_downcast(datatype::Integer, writer.getInputData(0)).setValue (5);
         wepa_datatype_downcast(datatype::String, writer.getInputData(1)).setValue ("the 5");
         wepa_datatype_downcast(datatype::Integer, writer.getInputData(2)).setValue (5 * 5);
         wepa_datatype_downcast(datatype::Float, writer.getInputData(3)).setValue (5.5);
         wepa_datatype_downcast(datatype::Date, writer.getInputData(4)).setValue ("5/5/2005T05:05:05", "%d/%m/%YT%H:%M");
         writer.execute ();

         BOOST_REQUIRE_EQUAL(conn0->operation_size(), 2);
         BOOST_REQUIRE_EQUAL (database.container_size(), 0);

         wepa_datatype_downcast(datatype::Integer, writer.getInputData(0)).setValue (6);
         wepa_datatype_downcast(datatype::String, writer.getInputData(1)).setValue ("the 6");
         wepa_datatype_downcast(datatype::Integer, writer.getInputData(2)).setValue (6 * 6);
         wepa_datatype_downcast(datatype::Float, writer.getInputData(3)).setValue (6.6);
         wepa_datatype_downcast(datatype::Date, writer.getInputData(4)).setValue ("6/6/2006T06:06:06", "%d/%m/%YT%H:%M");
         resultCode = writer.execute ();

         BOOST_REQUIRE_EQUAL (resultCode.getNumericCode(), test_dbms::MyDatabase::Successful);
         BOOST_REQUIRE_EQUAL (resultCode.successful(), true);
         BOOST_REQUIRE_EQUAL(conn0->operation_size(), 3);
         BOOST_REQUIRE_EQUAL (database.container_size(), 0);
      }
      catch (adt::Exception& ex) {
         logger::Logger::write (ex);
      }
   }

   BOOST_REQUIRE_EQUAL (conn0->operation_size(), 0);
   BOOST_REQUIRE_EQUAL (database.container_size(), 3);
   BOOST_REQUIRE_EQUAL (conn0->getCommitCounter(), 1);

   if (true) {
      dbms::GuardConnection guard (conn0);

      dbms::GuardStatement reader (guard, stReader);
      BOOST_REQUIRE_EQUAL (guard.getCountLinkedStatement(), 1);

      datatype::Integer& id = wepa_datatype_downcast(datatype::Integer, reader.getInputData(0));
      const datatype::Integer& integer = wepa_datatype_downcast(datatype::Integer, reader.getOutputData(1));

      id.setValue(5);
      BOOST_REQUIRE_NO_THROW (resultCode = reader.execute ());

      BOOST_REQUIRE_EQUAL (resultCode.successful(), true);

      int counter = 0;

      try {
      while (reader.fetch() == true) {
         ++ counter;
         BOOST_REQUIRE_EQUAL (integer.hasValue (), true);

         BOOST_REQUIRE_EQUAL (id.getValue () * id.getValue (), integer.getValue());
      }
      }
      catch (adt::Exception& ex) {
         std::cout << ex.what () << std::endl;
      }

      BOOST_REQUIRE_EQUAL (counter, 2);

      id.setValue (1111);
      resultCode = reader.execute ();
      BOOST_REQUIRE_EQUAL (resultCode.successful(), false);
      BOOST_REQUIRE_EQUAL (resultCode.notFound(), true);
      BOOST_REQUIRE_EQUAL (reader.fetch(), false);
   }

   BOOST_REQUIRE_EQUAL (database.container_size(), 3);
   BOOST_REQUIRE_EQUAL (conn0->getCommitCounter(), 1); // There was not commit due to reader operation

   if (true) {
      dbms::GuardConnection guard (conn0);

      dbms::GuardStatement eraser (guard, stEraser);

      datatype::Integer& id = wepa_datatype_downcast(datatype::Integer, eraser.getInputData(0));

      id.setValue (6);
      resultCode = eraser.execute();
      BOOST_REQUIRE_EQUAL (resultCode.successful(), true);

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 1);
      BOOST_REQUIRE_EQUAL (database.container_size(), 3);

      id.setValue (2);
      resultCode = eraser.execute();
      BOOST_REQUIRE_EQUAL (resultCode.successful(), true);

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 2);
      BOOST_REQUIRE_EQUAL (database.container_size(), 3);
   }

   BOOST_REQUIRE_EQUAL (database.container_size(), 1);
   BOOST_REQUIRE_EQUAL (conn0->getCommitCounter(), 2);

   LOG_DEBUG ("Enables termination");
   application.enableTermination();

   tr.join ();
}

BOOST_AUTO_TEST_CASE (dbms_write_rollback)
{
   MockApplication application ("dbms_write_rollback");

   test_dbms::MyDatabase database (application);

   application.disableTermination();

   mock::MockConnection* conn0 = static_cast <mock::MockConnection*> (database.createConnection("0", "0", "0"));
   dbms::Statement* rollbackWriter = database.createStatement("rollback_write", "write");
   dbms::Statement* noRollbackWriter = database.createStatement("no_rollback_write", "write", dbms::ActionOnError::Ignore);
   ResultCode resultCode;

   std::thread tr (std::ref (application));
   usleep (500);

   BOOST_REQUIRE_EQUAL (application.isRunning(), true);
   BOOST_REQUIRE_EQUAL (database.isRunning(), true);

   if (true) {
      dbms::GuardConnection guard (conn0);
      dbms::GuardStatement writer (guard, rollbackWriter);

      BOOST_REQUIRE_EQUAL (guard.getCountLinkedStatement(), 1);

      BOOST_REQUIRE_THROW(writer.getInputData(10), adt::RuntimeException);

      wepa_datatype_downcast(datatype::Integer, writer.getInputData(0)).setValue (2);
      wepa_datatype_downcast(datatype::String, writer.getInputData(1)).setValue ("the 2");
      wepa_datatype_downcast(datatype::Integer, writer.getInputData(2)).setValue (2 * 2);
      wepa_datatype_downcast(datatype::Float, writer.getInputData(3)).setValue (2.2);
      wepa_datatype_downcast(datatype::Date, writer.getInputData(4)).setValue ("2/2/2002T02:02:02", "%d/%m/%YT%H:%M");
      writer.execute ();

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 1);
      BOOST_REQUIRE_EQUAL (database.container_size(), 0);

      wepa_datatype_downcast(datatype::Integer, writer.getInputData(0)).setValue (5);
      wepa_datatype_downcast(datatype::String, writer.getInputData(1)).setValue ("the 5");
      wepa_datatype_downcast(datatype::Integer, writer.getInputData(2)).setValue (5 * 5);
      wepa_datatype_downcast(datatype::Float, writer.getInputData(3)).setValue (5.5);
      wepa_datatype_downcast(datatype::Date, writer.getInputData(4)).setValue ("5/5/2005T05:05:05", "%d/%m/%YT%H:%M");
      writer.execute ();

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 2);
      BOOST_REQUIRE_EQUAL (database.container_size(), 0);

      wepa_datatype_downcast(datatype::Integer, writer.getInputData(0)).setValue (6);
      wepa_datatype_downcast(datatype::String, writer.getInputData(1)).setValue ("the 6");
      wepa_datatype_downcast(datatype::Integer, writer.getInputData(2)).setValue (6 * 6);
      wepa_datatype_downcast(datatype::Float, writer.getInputData(3)).setValue (6.6);
      wepa_datatype_downcast(datatype::Date, writer.getInputData(4)).setValue ("6/6/2006T06:06:06", "%d/%m/%YT%H:%M");
      resultCode = writer.execute ();

      BOOST_REQUIRE_EQUAL (resultCode.getNumericCode(), test_dbms::MyDatabase::Successful);
      BOOST_REQUIRE_EQUAL (resultCode.successful(), true);
      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 3);
      BOOST_REQUIRE_EQUAL (database.container_size(), 0);
   }

   BOOST_REQUIRE_EQUAL (conn0->operation_size(), 0);
   BOOST_REQUIRE_EQUAL (database.container_size(), 3);
   BOOST_REQUIRE_EQUAL (conn0->getCommitCounter(), 1);
   BOOST_REQUIRE_EQUAL (conn0->getRollbackCounter(), 0);

   if (true) {
      dbms::GuardConnection guard (conn0);
      dbms::GuardStatement writer (guard, rollbackWriter);

      wepa_datatype_downcast(datatype::Integer, writer.getInputData(0)).setValue (8);
      wepa_datatype_downcast(datatype::String, writer.getInputData(1)).setValue ("the 8");
      wepa_datatype_downcast(datatype::Integer, writer.getInputData(2)).setValue (8 * 8);
      wepa_datatype_downcast(datatype::Float, writer.getInputData(3)).setValue (8.8);
      wepa_datatype_downcast(datatype::Date, writer.getInputData(4)).setValue ("8/8/2008T08:08:08", "%d/%m/%YT%H:%M");
      writer.execute ();

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 1);
      BOOST_REQUIRE_EQUAL (database.container_size(), 3);

      wepa_datatype_downcast(datatype::Integer, writer.getInputData(0)).setValue (666);
      wepa_datatype_downcast(datatype::String, writer.getInputData(1)).setValue ("the 666");
      wepa_datatype_downcast(datatype::Integer, writer.getInputData(2)).setValue (666 * 666);
      wepa_datatype_downcast(datatype::Float, writer.getInputData(3)).setValue (3.3);
      wepa_datatype_downcast(datatype::Date, writer.getInputData(4)).setValue ("3/3/2003T03:03:03", "%d/%m/%YT%H:%M");

      BOOST_REQUIRE_THROW(writer.execute (), dbms::DatabaseException);
   }

   BOOST_REQUIRE_EQUAL (conn0->operation_size(), 0);
   BOOST_REQUIRE_EQUAL (database.container_size(), 3);
   BOOST_REQUIRE_EQUAL (conn0->getCommitCounter(), 1);
   BOOST_REQUIRE_EQUAL (conn0->getRollbackCounter(), 1);

   if (true) {
      dbms::GuardConnection guard (conn0);
      dbms::GuardStatement writer (guard, noRollbackWriter);

      wepa_datatype_downcast(datatype::Integer, writer.getInputData(0)).setValue (8);
      wepa_datatype_downcast(datatype::String, writer.getInputData(1)).setValue ("the 8");
      wepa_datatype_downcast(datatype::Integer, writer.getInputData(2)).setValue (8 * 8);
      wepa_datatype_downcast(datatype::Float, writer.getInputData(3)).setValue (8.8);
      wepa_datatype_downcast(datatype::Date, writer.getInputData(4)).setValue ("8/8/2008T08:08:08", "%d/%m/%YT%H:%M");
      writer.execute ();

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 1);
      BOOST_REQUIRE_EQUAL (database.container_size(), 3);

      wepa_datatype_downcast(datatype::Integer, writer.getInputData(0)).setValue (666);
      wepa_datatype_downcast(datatype::String, writer.getInputData(1)).setValue ("the 666");
      wepa_datatype_downcast(datatype::Integer, writer.getInputData(2)).setValue (666 * 666);
      wepa_datatype_downcast(datatype::Float, writer.getInputData(3)).setValue (3.3);
      wepa_datatype_downcast(datatype::Date, writer.getInputData(4)).setValue ("3/3/2003T03:03:03", "%d/%m/%YT%H:%M");

      BOOST_REQUIRE_NO_THROW(resultCode = writer.execute ());

      BOOST_REQUIRE_EQUAL (resultCode.successful(), false);
      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 1);
      BOOST_REQUIRE_EQUAL (database.container_size(), 3);
   }

   BOOST_REQUIRE_EQUAL (conn0->operation_size(), 0);
   BOOST_REQUIRE_EQUAL (database.container_size(), 4);
   BOOST_REQUIRE_EQUAL (conn0->getCommitCounter(), 2);
   BOOST_REQUIRE_EQUAL (conn0->getRollbackCounter(), 1);

   LOG_DEBUG ("Enables termination");
   application.enableTermination();

   tr.join ();
}

BOOST_AUTO_TEST_CASE (dbms_erase_rollback)
{
   MockApplication application ("dbms_erase_rollback");

   test_dbms::MyDatabase database (application);

   application.disableTermination();

   mock::MockConnection* conn0 = static_cast <mock::MockConnection*> (database.createConnection("0", "0", "0"));
   dbms::Statement* stWriter = database.createStatement("writer", "write");
   dbms::Statement* rollbackEraser = database.createStatement("rollback_eraser", "delete");
   dbms::Statement* noRollbackEraser = database.createStatement("no_rollback_eraser", "delete", dbms::ActionOnError::Ignore);
   ResultCode resultCode;

   std::thread tr (std::ref (application));
   usleep (500);

   BOOST_REQUIRE_EQUAL (application.isRunning(), true);
   BOOST_REQUIRE_EQUAL (database.isRunning(), true);

   if (true) {
      dbms::GuardConnection guard (conn0);
      dbms::GuardStatement writer (guard, stWriter);

      BOOST_REQUIRE_EQUAL (guard.getCountLinkedStatement(), 1);

      BOOST_REQUIRE_THROW(writer.getInputData(10), adt::RuntimeException);

      wepa_datatype_downcast(datatype::Integer, writer.getInputData(0)).setValue (2);
      wepa_datatype_downcast(datatype::String, writer.getInputData(1)).setValue ("the 2");
      wepa_datatype_downcast(datatype::Integer, writer.getInputData(2)).setValue (2 * 2);
      wepa_datatype_downcast(datatype::Float, writer.getInputData(3)).setValue (2.2);
      wepa_datatype_downcast(datatype::Date, writer.getInputData(4)).setValue ("2/2/2002T02:02:02", "%d/%m/%YT%H:%M");
      writer.execute ();

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 1);
      BOOST_REQUIRE_EQUAL (database.container_size(), 0);

      wepa_datatype_downcast(datatype::Integer, writer.getInputData(0)).setValue (5);
      wepa_datatype_downcast(datatype::String, writer.getInputData(1)).setValue ("the 5");
      wepa_datatype_downcast(datatype::Integer, writer.getInputData(2)).setValue (5 * 5);
      wepa_datatype_downcast(datatype::Float, writer.getInputData(3)).setValue (5.5);
      wepa_datatype_downcast(datatype::Date, writer.getInputData(4)).setValue ("5/5/2005T05:05:05", "%d/%m/%YT%H:%M");
      writer.execute ();

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 2);
      BOOST_REQUIRE_EQUAL (database.container_size(), 0);

      wepa_datatype_downcast(datatype::Integer, writer.getInputData(0)).setValue (6);
      wepa_datatype_downcast(datatype::String, writer.getInputData(1)).setValue ("the 6");
      wepa_datatype_downcast(datatype::Integer, writer.getInputData(2)).setValue (6 * 6);
      wepa_datatype_downcast(datatype::Float, writer.getInputData(3)).setValue (6.6);
      wepa_datatype_downcast(datatype::Date, writer.getInputData(4)).setValue ("6/6/2006T06:06:06", "%d/%m/%YT%H:%M");
      resultCode = writer.execute ();

      BOOST_REQUIRE_EQUAL (resultCode.getNumericCode(), test_dbms::MyDatabase::Successful);
      BOOST_REQUIRE_EQUAL (resultCode.successful(), true);
      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 3);
      BOOST_REQUIRE_EQUAL (database.container_size(), 0);
   }

   BOOST_REQUIRE_EQUAL (conn0->operation_size(), 0);
   BOOST_REQUIRE_EQUAL (database.container_size(), 3);
   BOOST_REQUIRE_EQUAL (conn0->getCommitCounter(), 1);
   BOOST_REQUIRE_EQUAL (conn0->getRollbackCounter(), 0);

   if (true) {
      dbms::GuardConnection guard (conn0);
      dbms::GuardStatement eraser (guard, rollbackEraser);

      wepa_datatype_downcast(datatype::Integer, eraser.getInputData(0)).setValue (6);
      eraser.execute ();

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 1);
      BOOST_REQUIRE_EQUAL (database.container_size(), 3);

      wepa_datatype_downcast(datatype::Integer, eraser.getInputData(0)).setValue (666);
      BOOST_REQUIRE_THROW(eraser.execute (), dbms::DatabaseException);

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 1);
      BOOST_REQUIRE_EQUAL (database.container_size(), 3);
   }

   BOOST_REQUIRE_EQUAL (conn0->operation_size(), 0);
   BOOST_REQUIRE_EQUAL (database.container_size(), 3);
   BOOST_REQUIRE_EQUAL (conn0->getCommitCounter(), 1);
   BOOST_REQUIRE_EQUAL (conn0->getRollbackCounter(), 1);

   if (true) {
      dbms::GuardConnection guard (conn0);
      dbms::GuardStatement eraser (guard, noRollbackEraser);

      wepa_datatype_downcast(datatype::Integer, eraser.getInputData(0)).setValue (2);
      eraser.execute ();

      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 1);
      BOOST_REQUIRE_EQUAL (database.container_size(), 3);

      wepa_datatype_downcast(datatype::Integer, eraser.getInputData(0)).setValue (666);
      BOOST_REQUIRE_NO_THROW(resultCode = eraser.execute ());

      BOOST_REQUIRE_EQUAL (resultCode.successful(), false);
      BOOST_REQUIRE_EQUAL (resultCode.notFound(), true);
      BOOST_REQUIRE_EQUAL(conn0->operation_size(), 1);
      BOOST_REQUIRE_EQUAL (database.container_size(), 3);
   }

   BOOST_REQUIRE_EQUAL (conn0->operation_size(), 0);
   BOOST_REQUIRE_EQUAL (database.container_size(), 2);
   BOOST_REQUIRE_EQUAL (conn0->getCommitCounter(), 2);
   BOOST_REQUIRE_EQUAL (conn0->getRollbackCounter(), 1);

   LOG_DEBUG ("Enables termination");
   application.enableTermination();

   tr.join ();
}

BOOST_AUTO_TEST_CASE (dbms_set_max_commit)
{
   MockApplication application ("dbms_set_max_commit");

   test_dbms::MyDatabase database (application);

   application.disableTermination();

   mock::MockConnection* conn0 = static_cast <mock::MockConnection*> (database.createConnection("0", "0", "0"));
   dbms::Statement* stWriter = database.createStatement("the_write", "write");
   dbms::Statement* stReader = database.createStatement("the_read", "read");

   std::thread tr (std::ref (application));

   while (application.isRunning () == false);

   BOOST_REQUIRE_EQUAL (application.isRunning(), true);
   BOOST_REQUIRE_EQUAL (database.isRunning(), true);

   if (true) {
      GuardConnection connection (conn0);
      GuardStatement writer (connection, stWriter);

      BOOST_REQUIRE_EQUAL (connection.setMaxCommitPending(5), 0);

      for (int ii = 1; ii <= 15; ++ ii) {
         wepa_datatype_downcast(datatype::Integer, writer.getInputData(0)).setValue (ii);
         wepa_datatype_downcast(datatype::String, writer.getInputData(1)).setValue ("the ii");
         wepa_datatype_downcast(datatype::Integer, writer.getInputData(2)).setValue (ii * ii);
         wepa_datatype_downcast(datatype::Float, writer.getInputData(3)).setValue (100 / ii);
         wepa_datatype_downcast(datatype::Date, writer.getInputData(4)).setValue (adt::Second (ii));
         BOOST_REQUIRE_NO_THROW(writer.execute ());
      }

      BOOST_REQUIRE_EQUAL (conn0->operation_size(), 0);
      BOOST_REQUIRE_EQUAL (conn0->getCommitCounter(), 3);
      BOOST_REQUIRE_EQUAL (conn0->getRollbackCounter(), 0);
   }

   BOOST_REQUIRE_EQUAL (database.container_size(), 15);

   if (true) {
      GuardConnection connection (conn0);
      GuardStatement writer (connection, stWriter);

      connection.clearMaxCommitPending();

      for (int ii = 16; ii <= 30; ++ ii) {
         wepa_datatype_downcast(datatype::Integer, writer.getInputData(0)).setValue (ii);
         wepa_datatype_downcast(datatype::String, writer.getInputData(1)).setValue ("the ii");
         wepa_datatype_downcast(datatype::Integer, writer.getInputData(2)).setValue (ii * ii);
         wepa_datatype_downcast(datatype::Float, writer.getInputData(3)).setValue (100 / ii);
         wepa_datatype_downcast(datatype::Date, writer.getInputData(4)).setValue (adt::Second (ii));
         BOOST_REQUIRE_NO_THROW(writer.execute ());
      }
   }

   BOOST_REQUIRE_EQUAL (conn0->getCommitCounter(), 4);
   BOOST_REQUIRE_EQUAL (conn0->getRollbackCounter(), 0);
   BOOST_REQUIRE_EQUAL (conn0->operation_size(), 0);
   BOOST_REQUIRE_EQUAL (database.container_size(), 30);

   LOG_DEBUG ("Enables termination");
   application.enableTermination();

   tr.join ();
}

BOOST_AUTO_TEST_CASE (dbms_break_detected_executing)
{
   MockApplication application ("dbms_break_connection");

   test_dbms::MyDatabase database (application);

   application.disableTermination();

   mock::MockConnection* conn0 = static_cast <mock::MockConnection*> (database.createConnection("0", "0", "0"));
   dbms::Statement* stWriter = database.createStatement("the_write", "write");

   std::thread tr (std::ref (application));

   while (application.isRunning () == false);

   BOOST_REQUIRE_EQUAL (application.isRunning(), true);
   BOOST_REQUIRE_EQUAL (database.isRunning(), true);

   if (true) {
      GuardConnection connection (conn0);
      GuardStatement writer (connection, stWriter);

      BOOST_REQUIRE_EQUAL (conn0->getOpenCounter(), 1);
      BOOST_REQUIRE_EQUAL (conn0->getCloseCounter(), 0);

      int ii = 1;
      wepa_datatype_downcast(datatype::Integer, writer.getInputData(0)).setValue (ii);
      wepa_datatype_downcast(datatype::String, writer.getInputData(1)).setValue ("the ii");
      wepa_datatype_downcast(datatype::Integer, writer.getInputData(2)).setValue (ii * ii);
      wepa_datatype_downcast(datatype::Float, writer.getInputData(3)).setValue (100 / ii);
      wepa_datatype_downcast(datatype::Date, writer.getInputData(4)).setValue (adt::Second (ii));
      BOOST_REQUIRE_NO_THROW(writer.execute ());

      conn0->manualBreak ();

      ii = 2;
      wepa_datatype_downcast(datatype::Integer, writer.getInputData(0)).setValue (ii);
      wepa_datatype_downcast(datatype::String, writer.getInputData(1)).setValue ("the ii");
      wepa_datatype_downcast(datatype::Integer, writer.getInputData(2)).setValue (ii * ii);
      wepa_datatype_downcast(datatype::Float, writer.getInputData(3)).setValue (100 / ii);
      wepa_datatype_downcast(datatype::Date, writer.getInputData(4)).setValue (adt::Second (ii));

      // It will detect the lost connection but it will try to recover and it will get it.
      BOOST_REQUIRE_THROW(writer.execute (), dbms::DatabaseException);

      BOOST_REQUIRE_EQUAL (conn0->operation_size(), 0);
      BOOST_REQUIRE_EQUAL (conn0->getOpenCounter(), 2);
      BOOST_REQUIRE_EQUAL (conn0->getCloseCounter(), 1);
   }

   // It will recover the connection after it will detect
   BOOST_REQUIRE_EQUAL (conn0->isAvailable(), true);
   BOOST_REQUIRE_EQUAL (database.container_size(), 0);

   LOG_DEBUG ("Enables termination");
   application.enableTermination();

   tr.join ();
}

BOOST_AUTO_TEST_CASE (dbms_break_detected_locking)
{
   MockApplication application ("dbms_break_connection");

   test_dbms::MyDatabase database (application);

   application.disableTermination();

   mock::MockConnection* conn0 = static_cast <mock::MockConnection*> (database.createConnection("0", "0", "0"));
   dbms::Statement* stWriter = database.createStatement("the_write", "write");

   std::thread tr (std::ref (application));

   while (application.isRunning () == false);

   BOOST_REQUIRE_EQUAL (application.isRunning(), true);
   BOOST_REQUIRE_EQUAL (database.isRunning(), true);

   if (true) {
      GuardConnection connection (conn0);
      GuardStatement writer (connection, stWriter);

      BOOST_REQUIRE_EQUAL (conn0->getOpenCounter(), 1);
      BOOST_REQUIRE_EQUAL (conn0->getCloseCounter(), 0);

      int ii = 1;
      wepa_datatype_downcast(datatype::Integer, writer.getInputData(0)).setValue (ii);
      wepa_datatype_downcast(datatype::String, writer.getInputData(1)).setValue ("the ii");
      wepa_datatype_downcast(datatype::Integer, writer.getInputData(2)).setValue (ii * ii);
      wepa_datatype_downcast(datatype::Float, writer.getInputData(3)).setValue (100 / ii);
      wepa_datatype_downcast(datatype::Date, writer.getInputData(4)).setValue (adt::Second (ii));
      BOOST_REQUIRE_NO_THROW(writer.execute ());
   }

   conn0->manualBreak ();

   if (true) {
      // It will detect the lost connection but it will try to recover and it will get it.
      GuardConnection connection (conn0);
      GuardStatement writer (connection, stWriter);

      BOOST_REQUIRE_EQUAL (conn0->getOpenCounter(), 2);
      BOOST_REQUIRE_EQUAL (conn0->getCloseCounter(), 1);

      int ii = 2;
      wepa_datatype_downcast(datatype::Integer, writer.getInputData(0)).setValue (ii);
      wepa_datatype_downcast(datatype::String, writer.getInputData(1)).setValue ("the ii");
      wepa_datatype_downcast(datatype::Integer, writer.getInputData(2)).setValue (ii * ii);
      wepa_datatype_downcast(datatype::Float, writer.getInputData(3)).setValue (100 / ii);
      wepa_datatype_downcast(datatype::Date, writer.getInputData(4)).setValue (adt::Second (ii));

      BOOST_REQUIRE_NO_THROW(writer.execute ());
   }

   // It will recover the connection after it will detect
   BOOST_REQUIRE_EQUAL (conn0->isAvailable(), true);
   BOOST_REQUIRE_EQUAL (database.container_size(), 2);

   if (true) {
      // It will detect the lost connection but it will try to recover and it will get it.
      GuardConnection connection (conn0);

      conn0->manualBreak();

      BOOST_REQUIRE_THROW(GuardStatement writer (connection, stWriter), adt::RuntimeException);

      BOOST_REQUIRE_EQUAL (conn0->getOpenCounter(), 2);
      BOOST_REQUIRE_EQUAL (conn0->getCloseCounter(), 1);
   }

   if (true) {
      GuardConnection connection (conn0);
      GuardStatement writer (connection, stWriter);

      BOOST_REQUIRE_EQUAL (conn0->getOpenCounter(), 3);
      BOOST_REQUIRE_EQUAL (conn0->getCloseCounter(), 2);

      int ii = 3;
      wepa_datatype_downcast(datatype::Integer, writer.getInputData(0)).setValue (ii);
      wepa_datatype_downcast(datatype::String, writer.getInputData(1)).setValue ("the ii");
      wepa_datatype_downcast(datatype::Integer, writer.getInputData(2)).setValue (ii * ii);
      wepa_datatype_downcast(datatype::Float, writer.getInputData(3)).setValue (100 / ii);
      wepa_datatype_downcast(datatype::Date, writer.getInputData(4)).setValue (adt::Second (ii));

      BOOST_REQUIRE_NO_THROW(writer.execute ());
   }

   // It will recover the connection after it will detect
   BOOST_REQUIRE_EQUAL (conn0->isAvailable(), true);
   BOOST_REQUIRE_EQUAL (database.container_size(), 3);

   LOG_DEBUG ("Enables termination");
   application.enableTermination();

   tr.join ();
}

BOOST_AUTO_TEST_CASE (dbms_break_unrecovery_executing)
{
   MockApplication application ("dbms_break_unrecovery_executing");

   test_dbms::MyDatabase database (application);

   test_dbms::MyRecoveryHandler recoveryHandler;
   database.setFailRecoveryHandler(&recoveryHandler);

   application.disableTermination();

   mock::MockConnection* conn0 = static_cast <mock::MockConnection*> (database.createConnection("0", "0", "0"));
   dbms::Statement* stWriter = database.createStatement("the_write", "write");

   std::thread tr (std::ref (application));

   while (application.isRunning () == false);

   BOOST_REQUIRE_EQUAL (application.isRunning(), true);
   BOOST_REQUIRE_EQUAL (database.isRunning(), true);

   if (true) {
      GuardConnection connection (conn0);
      GuardStatement writer (connection, stWriter);

      BOOST_REQUIRE_EQUAL (conn0->getOpenCounter(), 1);
      BOOST_REQUIRE_EQUAL (conn0->getCloseCounter(), 0);

      int ii = 1;
      wepa_datatype_downcast(datatype::Integer, writer.getInputData(0)).setValue (ii);
      wepa_datatype_downcast(datatype::String, writer.getInputData(1)).setValue ("the ii");
      wepa_datatype_downcast(datatype::Integer, writer.getInputData(2)).setValue (ii * ii);
      wepa_datatype_downcast(datatype::Float, writer.getInputData(3)).setValue (100 / ii);
      wepa_datatype_downcast(datatype::Date, writer.getInputData(4)).setValue (adt::Second (ii));
      BOOST_REQUIRE_NO_THROW(writer.execute ());

      conn0->manualBreak ();
      conn0->avoidRecovery();

      ii = 2;
      wepa_datatype_downcast(datatype::Integer, writer.getInputData(0)).setValue (ii);
      wepa_datatype_downcast(datatype::String, writer.getInputData(1)).setValue ("the ii");
      wepa_datatype_downcast(datatype::Integer, writer.getInputData(2)).setValue (ii * ii);
      wepa_datatype_downcast(datatype::Float, writer.getInputData(3)).setValue (100 / ii);
      wepa_datatype_downcast(datatype::Date, writer.getInputData(4)).setValue (adt::Second (ii));

      // It will detect the lost connection but it will try to recover and it will get it.
      BOOST_REQUIRE_THROW(writer.execute (), dbms::DatabaseException);

      BOOST_REQUIRE_EQUAL (conn0->operation_size(), 0);
      BOOST_REQUIRE_EQUAL (conn0->getOpenCounter(), 1);
      BOOST_REQUIRE_EQUAL (conn0->getCloseCounter(), 1);
      BOOST_REQUIRE_EQUAL (recoveryHandler.thisWasUsed(), 1);
   }

   // It will recover the connection after it will detect
   BOOST_REQUIRE_EQUAL (conn0->isAvailable(), false);
   BOOST_REQUIRE_EQUAL (database.container_size(), 0);

   LOG_DEBUG ("Enables termination");
   application.enableTermination();

   tr.join ();
}

BOOST_AUTO_TEST_CASE (dbms_break_unrecovery_locking)
{
   MockApplication application ("dbms_break_connection");

   test_dbms::MyDatabase database (application);
   test_dbms::MyRecoveryHandler recoveryHandler;
   database.setFailRecoveryHandler(&recoveryHandler);

   application.disableTermination();

   mock::MockConnection* conn0 = static_cast <mock::MockConnection*> (database.createConnection("0", "0", "0"));
   dbms::Statement* stWriter = database.createStatement("the_write", "write");

   std::thread tr (std::ref (application));

   while (application.isRunning () == false);

   BOOST_REQUIRE_EQUAL (application.isRunning(), true);
   BOOST_REQUIRE_EQUAL (database.isRunning(), true);

   if (true) {
      GuardConnection connection (conn0);
      GuardStatement writer (connection, stWriter);

      BOOST_REQUIRE_EQUAL (conn0->getOpenCounter(), 1);
      BOOST_REQUIRE_EQUAL (conn0->getCloseCounter(), 0);

      int ii = 1;
      wepa_datatype_downcast(datatype::Integer, writer.getInputData(0)).setValue (ii);
      wepa_datatype_downcast(datatype::String, writer.getInputData(1)).setValue ("the ii");
      wepa_datatype_downcast(datatype::Integer, writer.getInputData(2)).setValue (ii * ii);
      wepa_datatype_downcast(datatype::Float, writer.getInputData(3)).setValue (100 / ii);
      wepa_datatype_downcast(datatype::Date, writer.getInputData(4)).setValue (adt::Second (ii));
      BOOST_REQUIRE_NO_THROW(writer.execute ());
   }

   conn0->manualBreak ();
   conn0->avoidRecovery();

   if (true) {
      BOOST_REQUIRE_THROW (GuardConnection connection (conn0), adt::RuntimeException);
      BOOST_REQUIRE_EQUAL (conn0->getOpenCounter(), 1);
      BOOST_REQUIRE_EQUAL (conn0->getCloseCounter(), 1);
      BOOST_REQUIRE_EQUAL (recoveryHandler.thisWasUsed(), 1);
   }

   // It will recover the connection after it will detect
   BOOST_REQUIRE_EQUAL (conn0->isAvailable(), false);
   BOOST_REQUIRE_EQUAL (database.container_size(), 1);

   LOG_DEBUG ("Enables termination");
   application.enableTermination();

   tr.join ();
}

BOOST_AUTO_TEST_CASE (dbms_dealing_with_nulls)
{
   MockApplication application ("dbms_dealing_with_nulls");

   test_dbms::MyDatabase database (application);

   application.disableTermination();

   mock::MockConnection* conn0 = static_cast <mock::MockConnection*> (database.createConnection("0", "0", "0"));
   dbms::Statement* stWriter = database.createStatement("the_write", "write");
   dbms::Statement* stReader = database.createStatement("the_read", "read");
   ResultCode resultCode;

   std::thread tr (std::ref (application));
   usleep (500);

   BOOST_REQUIRE_EQUAL (application.isRunning(), true);
   BOOST_REQUIRE_EQUAL (database.isRunning(), true);

   if (true) {
      try {
         dbms::GuardConnection guard (conn0);
         dbms::GuardStatement writer (guard, stWriter);

         wepa_datatype_downcast(datatype::Integer, writer.getInputData(0)).setValue (20);
         wepa_datatype_downcast(datatype::String, writer.getInputData(1)).setValue ("the 20");
         wepa_datatype_downcast(datatype::Integer, writer.getInputData(2)).isNull ();
         wepa_datatype_downcast(datatype::Float, writer.getInputData(3)).setValue (20.20);
         wepa_datatype_downcast(datatype::Date, writer.getInputData(4)).setValue ("2/2/2002T02:02:02", "%d/%m/%YT%H:%M");
         writer.execute ();

         BOOST_REQUIRE_EQUAL(conn0->operation_size(), 1);
         BOOST_REQUIRE_EQUAL (database.container_size(), 0);

         wepa_datatype_downcast(datatype::Integer, writer.getInputData(0)).setValue (25);
         wepa_datatype_downcast(datatype::String, writer.getInputData(1)).setValue ("the 25");
         wepa_datatype_downcast(datatype::Integer, writer.getInputData(2)).setValue (25 * 25);
         wepa_datatype_downcast(datatype::Float, writer.getInputData(3)).setValue (25.25);
         wepa_datatype_downcast(datatype::Date, writer.getInputData(4)).isNull ();
         ResultCode resultCode = writer.execute ();

         BOOST_REQUIRE_EQUAL(conn0->operation_size(), 2);
         BOOST_REQUIRE_EQUAL (database.container_size(), 0);

         BOOST_REQUIRE_EQUAL (resultCode.getNumericCode(), test_dbms::MyDatabase::Successful);
         BOOST_REQUIRE_EQUAL (resultCode.successful(), true);
         BOOST_REQUIRE_EQUAL(conn0->operation_size(), 2);
         BOOST_REQUIRE_EQUAL (database.container_size(), 0);
      }
      catch (adt::Exception& ex) {
         logger::Logger::write (ex);
      }
   }

   BOOST_REQUIRE_EQUAL (conn0->operation_size(), 0);
   BOOST_REQUIRE_EQUAL (database.container_size(), 2);
   BOOST_REQUIRE_EQUAL (conn0->getCommitCounter(), 1);

   if (true) {
      dbms::GuardConnection guard (conn0);

      dbms::GuardStatement reader (guard, stReader);
      BOOST_REQUIRE_EQUAL (guard.getCountLinkedStatement(), 1);

      datatype::Integer& id = wepa_datatype_downcast(datatype::Integer, reader.getInputData(0));
      const datatype::Integer& integer = wepa_datatype_downcast(datatype::Integer, reader.getOutputData(1));
      const datatype::Date& date = wepa_datatype_downcast(datatype::Date, reader.getOutputData(3));

      id.setValue(0);
      resultCode = reader.execute ();

      BOOST_REQUIRE_EQUAL (resultCode.successful(), true);

      BOOST_REQUIRE_EQUAL (reader.fetch (), true);

      BOOST_REQUIRE_EQUAL (id.getValue (), 20);
      BOOST_REQUIRE_EQUAL (integer.hasValue (), false);
      BOOST_REQUIRE_THROW (integer.getValue (), adt::RuntimeException);
      BOOST_REQUIRE_EQUAL (date.hasValue (), true);

      BOOST_REQUIRE_EQUAL (reader.fetch (), true);

      BOOST_REQUIRE_EQUAL (id.getValue (), 25);
      BOOST_REQUIRE_EQUAL (integer.hasValue (), true);
      BOOST_REQUIRE_EQUAL (date.hasValue (), false);
      BOOST_REQUIRE_THROW (date.getValue (), adt::RuntimeException);

      BOOST_REQUIRE_EQUAL (reader.fetch (), false);
   }

   LOG_DEBUG ("Enables termination");
   application.enableTermination();

   tr.join ();
}

