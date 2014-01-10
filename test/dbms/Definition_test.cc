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

#include <map>
#include <functional>
#include <mutex>
#include <thread>

#include <wepa/logger/Logger.hpp>
#include <wepa/logger/TraceMethod.hpp>
#include <wepa/logger/TtyWriter.hpp>

#include <wepa/app/Application.hpp>

#include <wepa/dbms/Database.hpp>
#include <wepa/dbms/Statement.hpp>
#include <wepa/dbms/Connection.hpp>
#include <wepa/dbms/GuardConnection.hpp>
#include <wepa/dbms/GuardStatement.hpp>
#include <wepa/dbms/StatementTranslator.hpp>
#include <wepa/dbms/FailRecoveryHandler.hpp>

#include <wepa/dbms/datatype/Integer.hpp>
#include <wepa/dbms/datatype/String.hpp>
#include <wepa/dbms/datatype/Float.hpp>
#include <wepa/dbms/datatype/Date.hpp>

namespace wepa {

namespace dbms {

namespace test {

struct MyRecord {
   int m_id;
   std::string m_name;
   int m_integer;
   float m_float;
   adt::Second m_time;
};

typedef std::map <int, MyRecord> Container;

class MyApplication : public app::Application {
public:
   MyApplication (const char* title);

   void disableTermination () noexcept { m_termination.lock (); }
   void operator ()();
   void enableTermination() noexcept { m_termination.unlock (); }

private:
   std::mutex m_termination;

   void run () throw (adt::RuntimeException);
};

class MyInput : public binder::Input {
public:
   explicit MyInput (datatype::Abstract& abstract) : binder::Input (abstract) {;}

private:
   void do_prepare (Statement* statement, Connection* connection, const int pos) throw (adt::RuntimeException, DatabaseException) {;}
   void do_release (Statement* statement) noexcept {;}
   void do_encode () throw (adt::RuntimeException) {;}
};

class MyOutput : public binder::Output {
public:
   explicit MyOutput (datatype::Abstract& abstract) : binder::Output (abstract) {;}

private:
   void do_prepare (Statement* statement, Connection* connection, const int pos) throw (adt::RuntimeException, DatabaseException) {;}
   void do_release (Statement* statement) noexcept {;}
   void do_decode () throw (adt::RuntimeException) {;}
   void do_write (const datatype::LongBlock&) throw (adt::RuntimeException, dbms::DatabaseException) {;}
};

class MyStatement : public Statement {
protected:
   MyStatement (Database& database, const char* name, const char* expression, const ActionOnError::_v actionOnError) :
      Statement (database, name, expression, actionOnError),
      m_id ("ID"),
      m_name ("name", 64),
      m_integer ("integer", true),
      m_float ("float"),
      m_time ("time", false)
   {;}

   MyRecord& getRecord () noexcept { return std::ref (m_record); }

protected:
   binder::BinderIf* m_binders [5];

   datatype::Integer m_id;
   datatype::String m_name;
   datatype::Integer m_integer;
   datatype::Float m_float;
   datatype::Date m_time;

private:
   MyRecord m_record;
};

class MyReadStatement : public MyStatement {
public:
   MyReadStatement (Database& database, const char* name, const char* expression, const ActionOnError::_v actionOnError);

private:
   std::vector <MyRecord> m_selection;
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

class MyConnection : public Connection {
public:
   MyConnection (Database& database, const std::string& name, const char* user, const char* password) :
      Connection (database, name, user, password),
      m_container (NULL)
   {
      m_commitCounter = m_rollbackCounter = m_openCounter = m_closeCounter = 0;
      m_avoidRecovery = false;
   }

   int operation_size () const noexcept { return m_operations.size (); }

   void manualBreak () noexcept { m_container = NULL; }
   bool isAvailable () const noexcept { return m_container != NULL; }
   void avoidRecovery () noexcept { m_avoidRecovery = true; }
   void clearAvoidRecovery () noexcept { m_avoidRecovery = false; }

   unsigned int getCommitCounter () const noexcept { return m_commitCounter; }
   unsigned int getRollbackCounter () const noexcept { return m_rollbackCounter; }
   unsigned int getOpenCounter () const noexcept { return m_openCounter; }
   unsigned int getCloseCounter () const noexcept { return m_closeCounter; }

   adt::StreamString asString () const noexcept {
      adt::StreamString result ("MyConnection {");
      result << Connection::asString ();
      result << " | CommitCounter=" << m_commitCounter;
      result << " | RollbackCounter=" << m_rollbackCounter;
      return result << " }";
   }

private:
   enum OpCode {  Write, Delete };
   typedef std::pair <OpCode, MyRecord> Operation;
   typedef std::vector <Operation> Operations;

   unsigned int m_commitCounter;
   unsigned int m_rollbackCounter;
   unsigned int m_openCounter;
   unsigned int m_closeCounter;

   Container* m_container;
   Operations m_operations;
   bool m_avoidRecovery;

   void open () throw (DatabaseException);
   void close () noexcept;
   void do_commit () noexcept ;
   void do_rollback () noexcept;

   friend class MyReadStatement;
   friend class MyWriteStatement;
};

class MyDatabase : public Database {
public:
   enum ErrorCode { NotFound, Successful, Lock, LostConnection };

   MyDatabase (app::Application& app) : Database (app, "map", app.getTitle ().c_str ()) {;}

   int container_size () const noexcept { return m_container.size (); }

private:
   Container m_container;

   bool notFound (const int errorCode) const noexcept { return errorCode == NotFound; }
   bool successful (const int errorCode) const noexcept { return errorCode == Successful; }
   bool locked (const int errorCode) const noexcept { return errorCode == Lock; }
   bool lostConnection (const int errorCode) const noexcept { return errorCode == LostConnection; }

   Statement* allocateStatement (const char* name, const std::string& expression, const ActionOnError::_v actionOnError)
      throw (adt::RuntimeException)
   {
      if (expression == "read" || expression == "READ")
         return new MyReadStatement (std::ref (*this), name, expression.c_str(), actionOnError);

      if (expression == "write" || expression == "delete")
         return new MyWriteStatement (std::ref (*this), name, expression.c_str(), actionOnError);

      return NULL;
   }

   Connection* allocateConnection (const std::string& name, const char* user, const char* password)
      throw (adt::RuntimeException)
   {
      return new MyConnection (std::ref (*this), name, user, password);
   }

   binder::Input* allocateInputBind (datatype::Abstract& data) throw (adt::RuntimeException) {
      return new MyInput (data);
   }

   binder::Output* allocateOutputBind (datatype::Abstract& data) throw (adt::RuntimeException) {
      return new MyOutput (data);
   }

   friend class MyConnection;

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
}

using namespace wepa;
using namespace wepa::dbms;


test::MyApplication::MyApplication (const char* title) : app::Application ("MyApp", title, "0.0")
{
   logger::Logger::initialize(new logger::TtyWriter);
}

void test::MyApplication::operator ()()
{
   try {
      start ();
   }
   catch (adt::Exception& ex) {
      logger::Logger::write(ex);
   }
}

void test::MyApplication::run ()
   throw (adt::RuntimeException)
{
   LOG_THIS_METHOD();

   LOG_DEBUG ("Waiting for enabled termination");
   m_termination.lock ();

   // It will be block until something frees the mutex by calling enableTermination
}

test::MyReadStatement::MyReadStatement (Database& database, const char* name, const char* expression, const ActionOnError::_v actionOnError) :
   MyStatement (database, name, expression, actionOnError),
   m_index (0)
{
   m_binders [0] = this->createBinderInput(m_id);
   m_binders [1] = this->createBinderOutput (m_name);
   m_binders [2] = this->createBinderOutput(m_integer);
   m_binders [3] = this->createBinderOutput(m_float);
   m_binders [4] = this->createBinderOutput(m_time);
}

dbms::ResultCode test::MyReadStatement::do_execute (dbms::Connection& connection)
   throw (adt::RuntimeException, DatabaseException)
{
   LOG_THIS_METHOD();

   m_index = 0;

   const MyConnection& _connection (static_cast <MyConnection&> (connection));

   m_selection.clear ();
   for (auto ii : *_connection.m_container) {
      if (ii.second.m_id >= m_id.getValue()) {
         m_selection.push_back(ii.second);
      }
   }

   LOG_DEBUG ("n-size:" << m_selection.size());

   return ResultCode (getDatabase(), (m_selection.size () == 0) ? MyDatabase::NotFound: MyDatabase::Successful);
}

bool test::MyReadStatement::do_fetch () throw (adt::RuntimeException, DatabaseException)
{
   LOG_THIS_METHOD();

   if (m_index >= m_selection.size ())
      return false;

   const MyRecord& record = m_selection [m_index ++];

   static_cast <datatype::Integer&> (m_binders [0]->getData()).setValue(record.m_id);
   static_cast <datatype::String&> (m_binders [1]->getData ()).setValue(record.m_name);
   static_cast <datatype::Integer&> (m_binders [2]->getData ()).setValue(record.m_integer);
   static_cast <datatype::Float&> (m_binders [3]->getData ()).setValue(record.m_float);
   static_cast <datatype::Date&> (m_binders [4]->getData ()).setValue(adt::Second (record.m_time));

   return true;
}

test::MyWriteStatement::MyWriteStatement (Database& database, const char* name, const char* expression, const ActionOnError::_v actionOnError) :
   MyStatement (database, name, expression, actionOnError)
{
   m_binders [0] = this->createBinderInput(m_id);
   m_binders [1] = this->createBinderInput (m_name);
   m_binders [2] = this->createBinderInput(m_integer);
   m_binders [3] = this->createBinderInput(m_float);
   m_binders [4] = this->createBinderInput(m_time);
}

dbms::ResultCode test::MyWriteStatement::do_execute (dbms::Connection& connection)
   throw (adt::RuntimeException, DatabaseException)
{
   LOG_THIS_METHOD();

   dbms::ResultCode result (getDatabase(), MyDatabase::Successful);

   if (connection.isAvailable() == false) {
      LOG_DEBUG (connection << " | Connection is not available");
      result.initialize(MyDatabase::LostConnection, NULL);
      return result;
   }

   MyConnection::OpCode opCode = (getExpression() == "write") ? MyConnection::Write: MyConnection::Delete;

   MyRecord record;

   record.m_id = static_cast <datatype::Integer&> (m_binders [0]->getData ()).getValue();

   if (record.m_id == 666) {
      result.initialize(MyDatabase::NotFound, NULL);
      return result;
   }

   if (opCode != MyConnection::Delete) {
      record.m_name = static_cast <datatype::String&> (m_binders [1]->getData ()).getValue();
      record.m_integer = static_cast <datatype::Integer&> (m_binders [2]->getData ()).getValue();
      record.m_float = static_cast <datatype::Float&> (m_binders [3]->getData ()).getValue();
      record.m_time = static_cast <datatype::Date&> (m_binders [4]->getData ()).getValue();
   }

   LOG_DEBUG ("ID = " << record.m_id);

   static_cast <MyConnection&> (connection).m_operations.push_back (MyConnection::Operation (opCode, record));

   return result;
}

void test::MyConnection::open ()
   throw (DatabaseException)
{
   m_operations.clear ();

   if (m_avoidRecovery == true) {
      WEPA_THROW_DB_EXCEPTION(dbms::ResultCode (getDatabase(), MyDatabase::LostConnection));
   }

   m_container = &reinterpret_cast <MyDatabase&> (getDatabase()).m_container;
   ++ m_openCounter;
   LOG_DEBUG ("OpenCounter=" << m_openCounter);
}

void test::MyConnection::close () noexcept
{
   ++ m_closeCounter;
   LOG_DEBUG ("CloseCounter=" << m_closeCounter);
   m_container = NULL;
}

void test::MyConnection::do_commit () noexcept
{
   LOG_THIS_METHOD();

   Container& container (*m_container);

   m_commitCounter ++;

   LOG_DEBUG("Commiting " << operation_size() << " changes");

   for (Operation& operation : m_operations) {
      switch (operation.first) {
      case Write:
         LOG_DEBUG ("Writing " << operation.second.m_id);
         container [operation.second.m_id] = operation.second;
         break;
      case Delete:
         {
            auto ii = container.find (operation.second.m_id);
            if (ii != container.end ()) {
               LOG_DEBUG ("Deleting " << operation.second.m_id);
               container.erase(ii);
            }
         }
         break;
      }
   }

   m_operations.clear ();
}

void test::MyConnection::do_rollback ()
   noexcept
{
   LOG_THIS_METHOD();
   m_rollbackCounter ++;
   m_operations.clear ();
}

const char* test::MyTranslator::apply (const char* statement)
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
   test::MyApplication application ("dbms_define_structure");

   test::MyDatabase database (application);

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
   test::MyApplication application ("dbms_define_structure");

   test::MyDatabase database (application);
   test::MyTranslator translator;

   dbms::Statement* st0 = database.createStatement("zero", "read");
   BOOST_REQUIRE_EQUAL (st0->getExpression (), "read");

   database.setStatementTranslator(&translator);

   dbms::Statement* st1 = database.createStatement("one", "read");
   BOOST_REQUIRE_EQUAL (st1->getExpression (), "READ");
}

BOOST_AUTO_TEST_CASE (dbms_link_guards)
{
   test::MyApplication application ("dbms_link_guards");

   test::MyDatabase database (application);

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
   test::MyApplication application ("dbms_write_and_read_and_delete");

   test::MyDatabase database (application);

   application.disableTermination();

   test::MyConnection* conn0 = static_cast <test::MyConnection*> (database.createConnection("0", "0", "0"));
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

         BOOST_REQUIRE_EQUAL (resultCode.getErrorCode(), test::MyDatabase::Successful);
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
      resultCode = reader.execute ();

      BOOST_REQUIRE_EQUAL (resultCode.successful(), true);

      int counter = 0;

      while (reader.fetch() == true) {
         ++ counter;
         BOOST_REQUIRE_EQUAL (id.getValue () * id.getValue (), integer.getValue());
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
   test::MyApplication application ("dbms_write_rollback");

   test::MyDatabase database (application);

   application.disableTermination();

   test::MyConnection* conn0 = static_cast <test::MyConnection*> (database.createConnection("0", "0", "0"));
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

      BOOST_REQUIRE_EQUAL (resultCode.getErrorCode(), test::MyDatabase::Successful);
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
   test::MyApplication application ("dbms_erase_rollback");

   test::MyDatabase database (application);

   application.disableTermination();

   test::MyConnection* conn0 = static_cast <test::MyConnection*> (database.createConnection("0", "0", "0"));
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

      BOOST_REQUIRE_EQUAL (resultCode.getErrorCode(), test::MyDatabase::Successful);
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
   test::MyApplication application ("dbms_set_max_commit");

   test::MyDatabase database (application);

   application.disableTermination();

   test::MyConnection* conn0 = static_cast <test::MyConnection*> (database.createConnection("0", "0", "0"));
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
   test::MyApplication application ("dbms_break_connection");

   test::MyDatabase database (application);

   application.disableTermination();

   test::MyConnection* conn0 = static_cast <test::MyConnection*> (database.createConnection("0", "0", "0"));
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
   test::MyApplication application ("dbms_break_connection");

   test::MyDatabase database (application);

   application.disableTermination();

   test::MyConnection* conn0 = static_cast <test::MyConnection*> (database.createConnection("0", "0", "0"));
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
   test::MyApplication application ("dbms_break_unrecovery_executing");

   test::MyDatabase database (application);

   test::MyRecoveryHandler recoveryHandler;
   database.setFailRecoveryHandler(&recoveryHandler);

   application.disableTermination();

   test::MyConnection* conn0 = static_cast <test::MyConnection*> (database.createConnection("0", "0", "0"));
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
   test::MyApplication application ("dbms_break_connection");

   test::MyDatabase database (application);
   test::MyRecoveryHandler recoveryHandler;
   database.setFailRecoveryHandler(&recoveryHandler);

   application.disableTermination();

   test::MyConnection* conn0 = static_cast <test::MyConnection*> (database.createConnection("0", "0", "0"));
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
