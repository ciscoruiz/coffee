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

#include <thread>

#include <wepa/adt/Second.hpp>

#include <wepa/logger/TraceMethod.hpp>
#include <wepa/logger/Logger.hpp>

#include <wepa/persistence/Repository.hpp>
#include <wepa/persistence/Storage.hpp>
#include <wepa/persistence/Loader.hpp>
#include <wepa/persistence/Recorder.hpp>
#include <wepa/persistence/Eraser.hpp>
#include <wepa/persistence/Creator.hpp>
#include <wepa/persistence/Object.hpp>
#include <wepa/persistence/Class.hpp>
#include <wepa/persistence/GuardClass.hpp>

#include <mock/MockLowLevelRecord.hpp>
#include <mock/MockDatabase.hpp>
#include <mock/MockConnection.hpp>

#include <wepa/dbms/GuardConnection.hpp>
#include <wepa/dbms/GuardStatement.hpp>
#include <wepa/dbms/Statement.hpp>

#include <wepa/dbms/datatype/Integer.hpp>
#include <wepa/dbms/datatype/String.hpp>
#include <wepa/dbms/datatype/Float.hpp>
#include <wepa/dbms/datatype/Date.hpp>

namespace wepa {

namespace test_persistence {

using namespace wepa::persistence;

/******
 * Low level data access
 */
class MyReadStatement : public dbms::Statement {
public:
   MyReadStatement (dbms::Database& database, const char* name, const char* expression, const dbms::ActionOnError::_v actionOnError) :
      dbms::Statement (database, name, expression, actionOnError),
      m_isValid (false)
   {;}

private:
   bool m_isValid;
   mock::MockLowLevelRecord m_selection;

   void do_prepare (dbms::Connection* connection) throw (adt::RuntimeException, dbms::DatabaseException) {;}
   dbms::ResultCode do_execute (dbms::Connection& connection) throw (adt::RuntimeException, dbms::DatabaseException);
   bool do_fetch () throw (adt::RuntimeException, dbms::DatabaseException);
};

class MyWriteStatement : public dbms::Statement {
public:
   MyWriteStatement (dbms::Database& database, const char* name, const char* expression, const dbms::ActionOnError::_v actionOnError) :
      dbms::Statement (database, name, expression, actionOnError),
      m_isValid (false)
   {;}

private:
   bool m_isValid;
   mock::MockLowLevelRecord m_selection;

   void do_prepare (dbms::Connection* connection) throw (adt::RuntimeException, dbms::DatabaseException) {;}
   dbms::ResultCode do_execute (dbms::Connection& connection) throw (adt::RuntimeException, dbms::DatabaseException);
   bool do_fetch () throw (adt::RuntimeException, dbms::DatabaseException) { return false; }
};


class MyDatabase : public mock::MockDatabase {
public:
   MyDatabase (app::Application& app) : mock::MockDatabase (app) { fillup (); }
   MyDatabase (const char* name) : mock::MockDatabase (name) { fillup (); }

private:
   void fillup () {
      mock::MockLowLevelRecord record;
      for (int ii = 0; ii < 10; ++ ii) {
         record.m_id = ii;
         record.m_name = adt::StreamString ("the name ") << ii;
         add (record);
      }
   }
   dbms::Statement* allocateStatement (const char* name, const std::string& expression, const dbms::ActionOnError::_v actionOnError)
      throw (adt::RuntimeException)
   {
      if (expression == "read" || expression == "READ" || expression == "none")
         return new MyReadStatement (std::ref (*this), name, expression.c_str(), actionOnError);

      if (expression == "write" || expression == "delete")
         return new MyWriteStatement (std::ref (*this), name, expression.c_str(), actionOnError);

      return NULL;
   }
};

class MockCustomerObject : public Object {
public:
   MockCustomerObject (Class& _class) : Object (_class) {
      m_id = 0;
   }

   int getId () const noexcept { return m_id; }
   const std::string& getName() const noexcept { return m_name; }

   void setId (const int value) noexcept { m_id = value; }
   void setName (const std::string& value) noexcept { m_name = value; }

   adt::StreamString asString () const noexcept {
      adt::StreamString result ("MockCustomerObject { ");
      result << m_id;
      return result += " }";
   }

private:
   int m_id;
   std::string m_name;

   void clear () noexcept {;}
};

class MockCustomerClass : public Class  {
public:
   MockCustomerClass () : Class ("MockCustomer") {;}

private:
   dbms::datatype::Abstract* do_createMember (const int columnNumber) const noexcept;
   Object* createObject () noexcept { return new MockCustomerObject (*this); }
};

struct MockCustomerLoader : public Loader {
public:
   MockCustomerLoader () : Loader ("MockCustomerLoader", 0) {;}

   void setId (GuardClass& _class, const int id) throw (adt::RuntimeException) { this->setMember(_class, 0, id); }

private:
   dbms::ResultCode do_apply (dbms::GuardStatement& statement, GuardClass& _class, Object& object) throw (adt::RuntimeException, dbms::DatabaseException);
   bool hasToRefresh (dbms::Connection&, GuardClass& _class, const Object& object) throw (adt::RuntimeException, dbms::DatabaseException) { return true; }
   bool isInputValue (const int columnNumber) const noexcept { return columnNumber == 0; }
   bool isPrimaryKeyComponent (const int columnNumber) const noexcept { return columnNumber == 0; }
   bool isOutputValue (const int columnNumber) const noexcept { return columnNumber >= 1; }
};

struct MockCustomerRecorder : public Recorder {
public:
   MockCustomerRecorder () : Recorder ("MockCustomerRecorder", 1) {;}

private:
   dbms::ResultCode do_apply (dbms::GuardStatement& statement, GuardClass& _class, Object& object) throw (adt::RuntimeException, dbms::DatabaseException);
   bool isInputValue (const int columnNumber) const noexcept { return true; }
   bool isPrimaryKeyComponent (const int columnNumber) const noexcept { return columnNumber == 0; }
   bool isOutputValue (const int columnNumber) const noexcept { return false; }
};

struct MockCustomerEraser : public Eraser {
public:
   MockCustomerEraser () : Eraser ("MockCustomerEraser", 2) {;}

private:
   dbms::ResultCode do_apply (dbms::GuardStatement& statement, GuardClass& _class, Object& object) throw (adt::RuntimeException, dbms::DatabaseException);
   bool isInputValue (const int columnNumber) const noexcept { return columnNumber == 0; }
   bool isPrimaryKeyComponent (const int columnNumber) const noexcept { return columnNumber == 0; }
   bool isOutputValue (const int columnNumber) const noexcept { return false; }
};

struct MockCustomerCreator : public Creator {
public:
   MockCustomerCreator () : Creator ("MockCustomerCreator", 3) {;}

   void setId (GuardClass& _class, const int id) throw (adt::RuntimeException) { this->setMember(_class, 0, id); }
   void setName (GuardClass& _class, const std::string& name) throw (adt::RuntimeException) { this->setMember(_class, 1, name); }

private:
   dbms::ResultCode do_apply (dbms::GuardStatement& statement, GuardClass& _class, Object& object) throw (adt::RuntimeException, dbms::DatabaseException);
   bool isInputValue (const int columnNumber) const noexcept { return true; }
   bool isPrimaryKeyComponent (const int columnNumber) const noexcept { return columnNumber == 0; }
   bool isOutputValue (const int columnNumber) const noexcept { return false; }
};

}
}

using namespace wepa;
using namespace wepa::dbms;

dbms::ResultCode test_persistence::MyReadStatement::do_execute (dbms::Connection& connection)
   throw (adt::RuntimeException, dbms::DatabaseException)
{
   m_isValid = false;

   mock::MockConnection& _connection (static_cast <mock::MockConnection&> (connection));

   int searchedId = wepa_datatype_downcast(dbms::datatype::Integer, getInputData(0)).getValue();

   for (auto ii : _connection.getContainer()) {
      if (ii.second.m_id == searchedId) {
         m_selection = ii.second;
         m_isValid = true;
      }
   }

   ResultCode result (getDatabase(), (m_isValid == false) ? MyDatabase::NotFound: MyDatabase::Successful);

   LOG_DEBUG ("Searching ID=" << searchedId << " | n-size=" << _connection.getContainer().size () << " | Result =" << result);

   return result;
}

bool test_persistence::MyReadStatement::do_fetch () throw (adt::RuntimeException, DatabaseException)
{
   if (m_isValid == true) {
      m_isValid = false;

      wepa_datatype_downcast(dbms::datatype::String, getOutputData(0)).setValue(m_selection.m_name);

      return true;
   }

   return false;
}

dbms::ResultCode test_persistence::MyWriteStatement::do_execute (dbms::Connection& connection)
   throw (adt::RuntimeException, dbms::DatabaseException)
{
   dbms::ResultCode result (getDatabase(), MyDatabase::Successful);

   mock::MockConnection::OpCode opCode = (getExpression() == "write") ? mock::MockConnection::Write: mock::MockConnection::Delete;

   mock::MockLowLevelRecord record;

   record.m_id = (wepa_datatype_downcast(dbms::datatype::Integer, getInputData(0)).getValue());

   if (record.m_id == 666) {
      result.initialize(MyDatabase::NotFound, NULL);
      return result;
   }

   if (opCode != mock::MockConnection::Delete) {
      record.m_name = (wepa_datatype_downcast(dbms::datatype::String, getInputData(1)).getValue());
   }

   LOG_DEBUG ("ID = " << record.m_id);

   static_cast <mock::MockConnection&> (connection).addOperation(opCode, record);

   return result;
}

datatype::Abstract* test_persistence::MockCustomerClass::do_createMember (const int columnNumber) const noexcept
{
   datatype::Abstract* result = NULL;

   switch (columnNumber) {
   case 0: result = new datatype::Integer ("id"); break;
   case 1: result = new datatype::String ("name", 64, dbms::datatype::Constraint::CanBeNull); break;
   }

   return result;
}

dbms::ResultCode test_persistence::MockCustomerLoader::do_apply (dbms::GuardStatement& statement, GuardClass& _class, Object& object)
   throw (adt::RuntimeException, dbms::DatabaseException)
{
   dbms::ResultCode result = statement.execute ();

   if (result.successful() == true) {
      if (statement.fetch ()) {
         MockCustomerObject& customer = static_cast <MockCustomerObject&> (object);
         customer.setId(this->readInteger(_class, 0));
         customer.setName(this->readCString(_class, 1));
      }
   }

   return result;
}

dbms::ResultCode test_persistence::MockCustomerRecorder::do_apply (dbms::GuardStatement& statement, GuardClass& _class, Object& object)
   throw (adt::RuntimeException, dbms::DatabaseException)
{
   test_persistence::MockCustomerObject& customer = static_cast <test_persistence::MockCustomerObject&> (object);

   this->setMember(_class, 0, customer.getId ());
   this->setMember (_class, 1, customer.getName ());

   return statement.execute ();
}

dbms::ResultCode test_persistence::MockCustomerEraser::do_apply (dbms::GuardStatement& statement, GuardClass& _class, Object& object)
   throw (adt::RuntimeException, dbms::DatabaseException)
{
   test_persistence::MockCustomerObject& customer = static_cast <test_persistence::MockCustomerObject&> (object);

   this->setMember(_class, 0, customer.getId ());

   return statement.execute ();
}

dbms::ResultCode test_persistence::MockCustomerCreator::do_apply (dbms::GuardStatement& statement, GuardClass& _class, Object& object)
   throw (adt::RuntimeException, dbms::DatabaseException)
{
   LOG_THIS_METHOD();

   test_persistence::MockCustomerObject& customer = static_cast <test_persistence::MockCustomerObject&> (object);

   customer.setId (this->readInteger(_class, 0));
   customer.setName(this->readCString(_class, 1));

   return dbms::ResultCode (statement->getDatabase(), mock::MockDatabase::Successful);
}

using namespace wepa;
using namespace wepa::mock;

BOOST_AUTO_TEST_CASE (persistence_storage_readonly)
{
   test_persistence::MyDatabase database ("persistence_storage_readonly");

   BOOST_REQUIRE_NO_THROW(database.externalInitialize ());

   BOOST_REQUIRE_EQUAL (database.isRunning(), true);

   dbms::Connection* conn0 = database.createConnection("0", "0", "0");
   dbms::Statement* stReader = database.createStatement("read_only", "read");

   persistence::Repository repository ("persistence_storage_readonly");
   persistence::Storage* ro_storage = repository.createStorage(0, "storage_readonly", persistence::Storage::AccessMode::ReadOnly);

   test_persistence::MockCustomerClass _class;

   BOOST_REQUIRE_NE (ro_storage, (void*) 0);

   test_persistence::MockCustomerLoader myLoader;

   if (true) {
      persistence::GuardClass guardCustomer (_class);

      BOOST_REQUIRE_NO_THROW(myLoader.initialize(guardCustomer, stReader));

      try{
         BOOST_REQUIRE_NO_THROW(myLoader.setId (guardCustomer, 6));
         test_persistence::MockCustomerObject& customer = static_cast <test_persistence::MockCustomerObject&> (ro_storage->load(*conn0, guardCustomer, myLoader));

         BOOST_REQUIRE_EQUAL (customer.getId(), 6);
         BOOST_REQUIRE_EQUAL (customer.getName(), "the name 6");

         test_persistence::MockCustomerObject& customer2 = static_cast <test_persistence::MockCustomerObject&> (ro_storage->load(*conn0, guardCustomer, myLoader));

         BOOST_REQUIRE_EQUAL (&customer, &customer2);
      }
      catch (adt::Exception& ex) {
         BOOST_REQUIRE_EQUAL (std::string ("no error"), ex.what ());
         std::cout << ex.what() << std::endl;
      }
   }

   if (true) {
      LOG_DEBUG ("Next access to cache ...");
      persistence::GuardClass guardCustomer (_class);

      BOOST_REQUIRE_NO_THROW(myLoader.setId (guardCustomer, 7));
      test_persistence::MockCustomerObject& customer = static_cast <test_persistence::MockCustomerObject&> (ro_storage->load(*conn0, guardCustomer, myLoader));

      BOOST_REQUIRE_EQUAL (customer.getId(), 7);
      BOOST_REQUIRE_EQUAL (customer.getName(), "the name 7");

      BOOST_REQUIRE_NO_THROW(myLoader.setId (guardCustomer, 77));
      BOOST_REQUIRE_THROW (ro_storage->load (*conn0, guardCustomer, myLoader), dbms::DatabaseException);
   }

   BOOST_REQUIRE_EQUAL (ro_storage->getFaultCounter(), 3);
   BOOST_REQUIRE_EQUAL (ro_storage->getHitCounter(), 1);
   BOOST_REQUIRE_EQUAL (ro_storage->getFaultCounter(), myLoader.getApplyCounter ());

   if (true) {
      LOG_DEBUG ("Reload record 6...");
      persistence::GuardClass guardCustomer (_class);

      BOOST_REQUIRE_NO_THROW(myLoader.setId (guardCustomer, 6));
      test_persistence::MockCustomerObject& customer = static_cast <test_persistence::MockCustomerObject&> (ro_storage->load(*conn0, guardCustomer, myLoader));

      BOOST_REQUIRE_EQUAL (customer.getId(), 6);
      BOOST_REQUIRE_EQUAL (customer.getName(), "the name 6");
   }

   BOOST_REQUIRE_EQUAL (ro_storage->getFaultCounter(), 3);
   BOOST_REQUIRE_EQUAL (ro_storage->getHitCounter(), 2);
   BOOST_REQUIRE_EQUAL (ro_storage->getFaultCounter(), myLoader.getApplyCounter ());

   BOOST_REQUIRE_NO_THROW(database.externalStop ());
}

BOOST_AUTO_TEST_CASE (persistence_storage_readonly_ignore_issue)
{
   test_persistence::MyDatabase database ("persistence_storage_readonly_ignore_issue");

   BOOST_REQUIRE_NO_THROW(database.externalInitialize ());

   BOOST_REQUIRE_EQUAL (database.isRunning(), true);

   dbms::Connection* conn0 = database.createConnection("0", "0", "0");
   dbms::Statement* stReader = database.createStatement("read_only_ignore_issue", "read", dbms::ActionOnError::Ignore);

   persistence::Repository repository ("persistence_storage_readonly_ignore_issue");
   persistence::Storage* ro_storage = repository.createStorage(0, "storage_readonly", persistence::Storage::AccessMode::ReadOnly);

   test_persistence::MockCustomerClass _class;

   BOOST_REQUIRE_NE (ro_storage, (void*) 0);

   test_persistence::MockCustomerLoader myLoader;

   if (true) {
      persistence::GuardClass guardCustomer (_class);

      BOOST_REQUIRE_NO_THROW(myLoader.initialize(guardCustomer, stReader));

      BOOST_REQUIRE_NO_THROW(myLoader.setId (guardCustomer, 6));
      test_persistence::MockCustomerObject& customer = static_cast <test_persistence::MockCustomerObject&> (ro_storage->load(*conn0, guardCustomer, myLoader));

      BOOST_REQUIRE_EQUAL (customer.getId(), 6);
      BOOST_REQUIRE_EQUAL (customer.getName(), "the name 6");

      BOOST_REQUIRE_NO_THROW(myLoader.setId (guardCustomer, 600));
      BOOST_REQUIRE_THROW(ro_storage->load(*conn0, guardCustomer, myLoader), dbms::DatabaseException);
   }

   BOOST_REQUIRE_EQUAL (ro_storage->getFaultCounter(), 2);
   BOOST_REQUIRE_EQUAL (ro_storage->getHitCounter(), 0);
   BOOST_REQUIRE_EQUAL (myLoader.getApplyCounter (), 2);

   BOOST_REQUIRE_NO_THROW(database.externalStop ());
}

BOOST_AUTO_TEST_CASE (persistence_storage_readwrite)
{
   test_persistence::MyDatabase database ("persistence_storage_readwrite");

   BOOST_REQUIRE_NO_THROW(database.externalInitialize ());

   BOOST_REQUIRE_EQUAL (database.isRunning(), true);

   dbms::Connection* conn0 = database.createConnection("0", "0", "0");
   dbms::Statement* stReader = database.createStatement("read_only", "read");

   BOOST_REQUIRE_NO_THROW(database.externalInitialize ());

   BOOST_REQUIRE_EQUAL (database.isRunning(), true);

   persistence::Repository repository ("persistence_storage_readwrite");
   persistence::Storage* rw_storage = repository.createStorage(0, "storage_readwrite", persistence::Storage::AccessMode::ReadWrite);

   test_persistence::MockCustomerClass _class;

   BOOST_REQUIRE_NE (rw_storage, (void*) 0);

   test_persistence::MockCustomerLoader myLoader;

   if (true) {
      persistence::GuardClass guardCustomer (_class);

      BOOST_REQUIRE_NO_THROW(myLoader.initialize(guardCustomer, stReader));

      BOOST_REQUIRE_NO_THROW(myLoader.setId (guardCustomer, 9));
      test_persistence::MockCustomerObject& customer = static_cast <test_persistence::MockCustomerObject&> (rw_storage->load(*conn0, guardCustomer, myLoader));

      BOOST_REQUIRE_EQUAL (customer.getId(), 9);
      BOOST_REQUIRE_EQUAL (customer.getName(), "the name 9");

      test_persistence::MockCustomerObject& customer2 = static_cast <test_persistence::MockCustomerObject&> (rw_storage->load(*conn0, guardCustomer, myLoader));
      BOOST_REQUIRE_EQUAL (&customer, &customer2);

      BOOST_REQUIRE_EQUAL (rw_storage->getFaultCounter(), 1);
      BOOST_REQUIRE_EQUAL (rw_storage->getHitCounter(), 1);
      BOOST_REQUIRE_EQUAL (myLoader.getApplyCounter (), 2);

      BOOST_REQUIRE_EQUAL (rw_storage->release (guardCustomer, customer), false);
      BOOST_REQUIRE_EQUAL (rw_storage->release (guardCustomer, customer2), true);
   }

   if (true) {
      LOG_DEBUG ("Reload record 9...");
      persistence::GuardClass guardCustomer (_class);

      BOOST_REQUIRE_NO_THROW(myLoader.setId (guardCustomer, 9));
      test_persistence::MockCustomerObject& customer = static_cast <test_persistence::MockCustomerObject&> (rw_storage->load(*conn0, guardCustomer, myLoader));

      BOOST_REQUIRE_EQUAL (customer.getId(), 9);
      BOOST_REQUIRE_EQUAL (customer.getName(), "the name 9");

      mock::MockLowLevelRecord record;
      record.m_id = 9;
      record.m_name = "updated name 9";
      database.update (record);

      test_persistence::MockCustomerObject& customer2 = static_cast <test_persistence::MockCustomerObject&> (rw_storage->load(*conn0, guardCustomer, myLoader));
      BOOST_REQUIRE_EQUAL (&customer, &customer2);

      BOOST_REQUIRE_EQUAL (customer.getId(), 9);
      BOOST_REQUIRE_EQUAL (customer.getName(), "updated name 9");

      BOOST_REQUIRE_EQUAL (rw_storage->getFaultCounter(), 2);
      BOOST_REQUIRE_EQUAL (rw_storage->getHitCounter(), 2);
      BOOST_REQUIRE_EQUAL (myLoader.getApplyCounter (), 4);

      BOOST_REQUIRE_EQUAL (rw_storage->release (guardCustomer, customer), false);
      BOOST_REQUIRE_EQUAL (rw_storage->release (guardCustomer, customer2), true);
   }

   BOOST_REQUIRE_NO_THROW(database.externalStop ());
}

BOOST_AUTO_TEST_CASE (persistence_storage_readonly_avoid_write)
{
   test_persistence::MyDatabase database ("persistence_storage_readonly");

   BOOST_REQUIRE_NO_THROW(database.externalInitialize ());

   BOOST_REQUIRE_EQUAL (database.isRunning(), true);

   dbms::Connection* conn0 = database.createConnection("0", "0", "0");
   dbms::Statement* stReader = database.createStatement("read_only", "read");
   dbms::Statement* stWriter = database.createStatement("writer", "write");

   persistence::Repository repository ("persistence_storage_readonly_avoid_write");
   persistence::Storage* ro_storage = repository.createStorage(0, "storage_readonly", persistence::Storage::AccessMode::ReadOnly);

   test_persistence::MockCustomerClass _class;

   BOOST_REQUIRE_NE (ro_storage, (void*) 0);

   test_persistence::MockCustomerLoader myLoader;
   test_persistence::MockCustomerRecorder myRecorder;

   if (true) {
      persistence::GuardClass guardCustomer (_class);

      BOOST_REQUIRE_NO_THROW(myLoader.initialize(guardCustomer, stReader));

      BOOST_REQUIRE_NO_THROW(myLoader.setId (guardCustomer, 6));
      test_persistence::MockCustomerObject& customer = static_cast <test_persistence::MockCustomerObject&> (ro_storage->load(*conn0, guardCustomer, myLoader));

      BOOST_REQUIRE_EQUAL (customer.getId(), 6);
      BOOST_REQUIRE_EQUAL (customer.getName(), "the name 6");
   }

   if (true) {
      persistence::GuardClass guardCustomer (_class);

      BOOST_REQUIRE_NO_THROW(myRecorder.initialize(guardCustomer, stWriter));

      try {
         ro_storage->save(*conn0, guardCustomer, myRecorder);
         BOOST_REQUIRE_EQUAL (true, false);
      }
      catch (adt::RuntimeException& ex) {
         std::cout << ex.asString () << std::endl;
         std::string error = ex.what ();
         BOOST_REQUIRE_NE(error.find ("can not write on a read-only storage"), std::string::npos);
      }
   }

   BOOST_REQUIRE_NO_THROW(database.externalStop ());
}

BOOST_AUTO_TEST_CASE (persistence_storage_write)
{
   test_persistence::MyDatabase database ("persistence_storage_write");

   BOOST_REQUIRE_NO_THROW(database.externalInitialize ());

   mock::MockConnection* conn0 = static_cast <mock::MockConnection*> (database.createConnection("0", "0", "0"));
   dbms::Statement* stReader = database.createStatement("read_only", "read");
   dbms::Statement* stWriter = database.createStatement("writer", "write");

   BOOST_REQUIRE_EQUAL (database.isRunning(), true);

   persistence::Repository repository ("persistence_storage_write");
   persistence::Storage* wr_storage = repository.createStorage(0, "storage_write", persistence::Storage::AccessMode::ReadWrite);

   test_persistence::MockCustomerClass _class;

   BOOST_REQUIRE_NE (wr_storage, (void*) 0);

   test_persistence::MockCustomerLoader myLoader;
   test_persistence::MockCustomerRecorder myRecorder;

   if (true) {
      persistence::GuardClass guardCustomer (_class);

      BOOST_REQUIRE_NO_THROW(myLoader.initialize(guardCustomer, stReader));

      BOOST_REQUIRE_NO_THROW(myLoader.setId (guardCustomer, 2));
      test_persistence::MockCustomerObject& customer = static_cast <test_persistence::MockCustomerObject&> (wr_storage->load(*conn0, guardCustomer, myLoader));

      BOOST_REQUIRE_EQUAL (customer.getId(), 2);
      BOOST_REQUIRE_EQUAL (customer.getName(), "the name 2");

      BOOST_REQUIRE_NO_THROW(myRecorder.initialize(guardCustomer, stWriter));

      customer.setName ("updated name 2");

      BOOST_REQUIRE_THROW(myRecorder.getObject(), adt::RuntimeException);
      BOOST_REQUIRE_NO_THROW(myRecorder.setObject(customer));

      BOOST_REQUIRE_NO_THROW (wr_storage->save (*conn0, guardCustomer, myRecorder));

      BOOST_REQUIRE_EQUAL (conn0->getCommitCounter(), 1);
      BOOST_REQUIRE_EQUAL (conn0->operation_size(), 0);
   }

   BOOST_REQUIRE_EQUAL (conn0->operation_size(), 0);
   BOOST_REQUIRE_EQUAL (database.container_size(), 10);

   if (true) {
      persistence::GuardClass guardCustomer (_class);

      BOOST_REQUIRE_NO_THROW(myLoader.setId (guardCustomer, 2));
      test_persistence::MockCustomerObject& customer = static_cast <test_persistence::MockCustomerObject&> (wr_storage->load(*conn0, guardCustomer, myLoader));

      BOOST_REQUIRE_EQUAL (customer.getId(), 2);
      BOOST_REQUIRE_EQUAL (customer.getName(), "updated name 2");
   }

   BOOST_REQUIRE_NO_THROW(database.externalStop ());
}

BOOST_AUTO_TEST_CASE (persistence_storage_erase)
{
   test_persistence::MyDatabase database ("dbms_with_app");

   BOOST_REQUIRE_NO_THROW(database.externalInitialize ());

   BOOST_REQUIRE_EQUAL (database.isRunning(), true);

   mock::MockConnection* conn0 = static_cast <mock::MockConnection*> (database.createConnection("0", "0", "0"));
   dbms::Statement* stReader = database.createStatement("read_only", "read");
   dbms::Statement* stEraser = database.createStatement("eraser", "delete");

   persistence::Repository repository ("persistence_storage_erase");
   persistence::Storage* wr_storage = repository.createStorage(0, "storage_erase", persistence::Storage::AccessMode::ReadWrite);

   test_persistence::MockCustomerClass _class;

   BOOST_REQUIRE_NE (wr_storage, (void*) 0);

   test_persistence::MockCustomerLoader myLoader;
   test_persistence::MockCustomerEraser myEraser;

   if (true) {
      persistence::GuardClass guardCustomer (_class);

      BOOST_REQUIRE_NO_THROW(myLoader.initialize(guardCustomer, stReader));

      BOOST_REQUIRE_NO_THROW(myLoader.setId (guardCustomer, 2));
      test_persistence::MockCustomerObject& customer = static_cast <test_persistence::MockCustomerObject&> (wr_storage->load(*conn0, guardCustomer, myLoader));

      BOOST_REQUIRE_EQUAL (customer.getId(), 2);
      BOOST_REQUIRE_EQUAL (customer.getName(), "the name 2");

      BOOST_REQUIRE_NO_THROW(myEraser.initialize(guardCustomer, stEraser));

      BOOST_REQUIRE_THROW(myEraser.getObject(), adt::RuntimeException);

      BOOST_REQUIRE_NO_THROW(customer.getPrimaryKey());

      myEraser.setObject(customer);

      BOOST_REQUIRE_NO_THROW (myEraser.getObject());
      BOOST_REQUIRE_NO_THROW(myEraser.getObject().getPrimaryKey());

      try {
         wr_storage->erase (*conn0, guardCustomer, myEraser);
      }
      catch (adt::RuntimeException& ex) {
         std::cout << ex.asString () << std::endl;
         BOOST_REQUIRE_EQUAL (std::string ("none"), ex.what ());
      }

      BOOST_REQUIRE_EQUAL (conn0->getCommitCounter(), 1);
      BOOST_REQUIRE_EQUAL (conn0->operation_size(), 0);
   }

   BOOST_REQUIRE_EQUAL (conn0->operation_size(), 0);
   BOOST_REQUIRE_EQUAL (database.container_size(), 9);

   if (true) {
      persistence::GuardClass guardCustomer (_class);

      BOOST_REQUIRE_NO_THROW(myLoader.setId (guardCustomer, 4));
      test_persistence::MockCustomerObject& customer = static_cast <test_persistence::MockCustomerObject&> (wr_storage->load(*conn0, guardCustomer, myLoader));

      BOOST_REQUIRE_EQUAL (customer.getId(), 4);
      BOOST_REQUIRE_EQUAL (customer.getName(), "the name 4");

      BOOST_REQUIRE_NO_THROW (wr_storage->load(*conn0, guardCustomer, myLoader));

      try {
         myEraser.setObject(customer);
         wr_storage->erase (*conn0, guardCustomer, myEraser);
         BOOST_REQUIRE_EQUAL (true, false);
      }
      catch (adt::RuntimeException& ex) {
         std::cout << ex.asString () << std::endl;
         std::string error = ex.what ();
         BOOST_REQUIRE_NE(error.find ("due to multiple references"), std::string::npos);
      }
   }

   BOOST_REQUIRE_NO_THROW(database.externalStop ());
}

BOOST_AUTO_TEST_CASE (persistence_storage_create)
{
   test_persistence::MyDatabase database ("persistence_storage_create");

   BOOST_REQUIRE_NO_THROW(database.externalInitialize());

   BOOST_REQUIRE_EQUAL (database.isRunning(), true);

   mock::MockConnection* conn0 = static_cast <mock::MockConnection*> (database.createConnection("0", "0", "0"));

   BOOST_REQUIRE_NE (conn0, (void*) 0);

   dbms::Statement* stReader = database.createStatement("read_only", "read");

   BOOST_REQUIRE_NE (stReader, (void*) 0);

   dbms::Statement* stCreator = database.createStatement("creator", "none");

   BOOST_REQUIRE_NE (stCreator, (void*) 0);

   dbms::Statement* stWriter = database.createStatement("writer", "write");

   persistence::Repository repository ("persistence_storage_create");
   persistence::Storage* wr_storage = repository.createStorage(0, "storage_create", persistence::Storage::AccessMode::ReadWrite);

   test_persistence::MockCustomerClass _class;

   BOOST_REQUIRE_NE (wr_storage, (void*) 0);

   test_persistence::MockCustomerRecorder myRecorder;
   test_persistence::MockCustomerCreator myCreator;
   test_persistence::MockCustomerLoader myLoader;

   if (true) {
      persistence::GuardClass guardCustomer (_class);

      BOOST_CHECK_NO_THROW(myCreator.initialize(guardCustomer, stCreator));

      myCreator.setId(guardCustomer, 1000);
      myCreator.setName(guardCustomer, "the name 1000");
      test_persistence::MockCustomerObject& customer = static_cast <test_persistence::MockCustomerObject&> (wr_storage->create (*conn0, guardCustomer, myCreator));

      LOG_DEBUG ("Customer has been created");

      BOOST_REQUIRE_NO_THROW (myRecorder.initialize(guardCustomer, stWriter));
      myRecorder.setObject (customer);

      BOOST_REQUIRE_NO_THROW (wr_storage->save (*conn0, guardCustomer, myRecorder));

      BOOST_REQUIRE_EQUAL (conn0->getCommitCounter(), 1);
      BOOST_REQUIRE_EQUAL (conn0->operation_size(), 0);
   }

   BOOST_REQUIRE_EQUAL (database.container_size(), 11);

   if (true) {
      persistence::GuardClass guardCustomer (_class);

      BOOST_REQUIRE_NO_THROW(myLoader.initialize(guardCustomer, stReader));

      BOOST_REQUIRE_NO_THROW(myLoader.setId (guardCustomer, 1000));
      test_persistence::MockCustomerObject& customer = static_cast <test_persistence::MockCustomerObject&> (wr_storage->load(*conn0, guardCustomer, myLoader));

      BOOST_REQUIRE_EQUAL (customer.getId(), 1000);
      BOOST_REQUIRE_EQUAL (customer.getName(), "the name 1000");
   }

   if (true) {
      persistence::GuardClass guardCustomer (_class);

      // Accessor already initialized
      BOOST_CHECK_THROW(myCreator.initialize(guardCustomer, stCreator), adt::RuntimeException);

      myCreator.setId(guardCustomer, 1000);
      myCreator.setName(guardCustomer, "the name 1000");

      try {
         wr_storage->create (*conn0, guardCustomer, myCreator);
         BOOST_REQUIRE_EQUAL (false, true);
      }
      catch (adt::RuntimeException& ex) {
         std::cout << ex.asString() << std::endl;
         BOOST_REQUIRE_NE (ex.asString ().find ("PrimaryKey is already registered"), std::string::npos);
      }
   }

   BOOST_REQUIRE_NO_THROW(database.externalStop());
}
