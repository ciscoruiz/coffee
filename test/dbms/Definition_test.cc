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
   time_t m_time;
};

typedef std::map <int, MyRecord> Container;

class MyApplication : public app::Application {
public:
   MyApplication ();

   void disableTermination () throw () { m_termination.lock (); }
   void operator ()();
   void enableTermination() throw () { m_termination.unlock (); }

private:
   std::mutex m_termination;

   void run () throw (adt::RuntimeException);
};

class MyInput : public binder::Input {
public:
   explicit MyInput (datatype::Abstract& abstract) : binder::Input (abstract) {;}

private:
   void do_prepare (Statement* statement, Connection* connection, const int pos) throw (adt::RuntimeException, DatabaseException) {;}
   void do_release (Statement* statement) throw () {;}
   void do_encode () throw (adt::RuntimeException) {;}
};

class MyOutput : public binder::Output {
public:
   explicit MyOutput (datatype::Abstract& abstract) : binder::Output (abstract) {;}

private:
   void do_prepare (Statement* statement, Connection* connection, const int pos) throw (adt::RuntimeException, DatabaseException) {;}
   void do_release (Statement* statement) throw () {;}
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
      m_time ("time")
   {;}

   MyRecord& getRecord () throw () { return std::ref (m_record); }

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
   ResultCode do_execute (Connection* connection) throw (adt::RuntimeException, DatabaseException);
   bool do_fetch () throw (adt::RuntimeException, DatabaseException);
};

class MyWriteStatement : public MyStatement {
public:
   MyWriteStatement (Database& database, const char* name, const char* expression, const ActionOnError::_v actionOnError);

private:
   void do_prepare (Connection* connection) throw (adt::RuntimeException, DatabaseException) {;}
   ResultCode do_execute (Connection* connection) throw (adt::RuntimeException, DatabaseException);
   bool do_fetch () throw (adt::RuntimeException, DatabaseException) { return false; }
};

class MyConnection : public Connection {
public:
   MyConnection (Database& database, const std::string& name, const char* user, const char* password) :
      Connection (database, name, user, password),
      m_container (NULL)
   {;}

private:
   enum OpCode {  Write, Delete };
   typedef std::pair <OpCode, MyRecord> Operation;
   typedef std::vector <Operation> Operations;

   Container* m_container;
   Operations m_operations;

   bool isAvailable () const throw () { return m_container != NULL; }
   void open () throw (DatabaseException);
   void close () throw () { m_container = NULL; }
   void do_commit () throw () ;
   void do_rollback () throw () { m_operations.clear ();}

   friend class MyReadStatement;
   friend class MyWriteStatement;
};

class MyDatabase : public Database {
public:
   enum ErrorCode { NotFound, Successful, Lock, LostConnection };

   MyDatabase (app::Application& app) : Database (app, "map", "my_database") {;}

private:
   Container m_container;

   bool notFound (const int errorCode) const throw () { return errorCode == NotFound; }
   bool successful (const int errorCode) const throw () { return errorCode == Successful; }
   bool locked (const int errorCode) const throw () { return errorCode == Lock; }
   bool lostConnection (const int errorCode) const throw () { return errorCode == LostConnection; }

   Statement* allocateStatement (const char* name, const std::string& expression, const ActionOnError::_v actionOnError)
      throw (adt::RuntimeException)
   {
      if (expression == "read")
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

}
}
}

using namespace wepa;
using namespace wepa::dbms;


test::MyApplication::MyApplication () : app::Application ("MyApp", "the application", "0.0")
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

dbms::ResultCode test::MyReadStatement::do_execute (dbms::Connection* connection)
   throw (adt::RuntimeException, DatabaseException)
{
   m_index = 0;

   const MyConnection& _connection (static_cast <MyConnection&> (*connection));

   m_selection.clear ();
   for (auto ii : *_connection.m_container) {
      if (ii.second.m_id > m_id.getValue()) {
         m_selection.push_back(ii.second);
      }
   }

   return ResultCode (getDatabase(), MyDatabase::Successful);
}

bool test::MyReadStatement::do_fetch () throw (adt::RuntimeException, DatabaseException)
{
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

dbms::ResultCode test::MyWriteStatement::do_execute (dbms::Connection* connection)
   throw (adt::RuntimeException, DatabaseException)
{
   MyConnection::OpCode opCode = (getName () == "write") ? MyConnection::Write: MyConnection::Delete;

   MyRecord record;
   dbms::ResultCode result (getDatabase(), MyDatabase::Successful);

   record.m_id = static_cast <datatype::Integer&> (m_binders [0]->getData ()).getValue();

   if (opCode != MyConnection::Delete) {
      record.m_name = static_cast <datatype::String&> (m_binders [1]->getData ()).getValue();
      record.m_integer = static_cast <datatype::Integer&> (m_binders [2]->getData ()).getValue();
      record.m_float = static_cast <datatype::Float&> (m_binders [3]->getData ()).getValue();
      record.m_time = static_cast <datatype::Date&> (m_binders [4]->getData ()).getSecondValue().getValue();
   }

   static_cast <MyConnection*> (connection)->m_operations.push_back (MyConnection::Operation (opCode, record));

   return result;
}

void test::MyConnection::open ()
   throw (DatabaseException)
{
   m_operations.clear ();
   m_container = &reinterpret_cast <MyDatabase&> (getDatabase()).m_container;
}

void test::MyConnection::do_commit () throw ()
{
   Container& container (*m_container);

   for (Operation& operation : m_operations) {
      switch (operation.first) {
      case Write:
         container [operation.second.m_id] = operation.second;
         break;
      case Delete:
      {
         auto ii = container.find (operation.second.m_id);
         if (ii != container.end ())
            container.erase(ii);
      }
      break;
      }
   }
}

BOOST_AUTO_TEST_CASE (dbms_find_stuff)
{
   test::MyApplication application;

   test::MyDatabase database (application);

   application.disableTermination();

   std::thread tr (std::ref (application));

   usleep (500);

   BOOST_REQUIRE_EQUAL (application.isRunning(), true);
   BOOST_REQUIRE_EQUAL (database.isRunning(), true);

   dbms::Statement* write = database.createStatement("the_write", "write");



   LOG_DEBUG ("Enables termination");
   application.enableTermination();

   tr.join ();
}

