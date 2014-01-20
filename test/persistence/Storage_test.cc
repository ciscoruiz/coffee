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

#include <wepa/persistence/Repository.hpp>
#include <wepa/persistence/Storage.hpp>
#include <wepa/persistence/Loader.hpp>
#include <wepa/persistence/Object.hpp>
#include <wepa/persistence/Class.hpp>
#include <wepa/persistence/GuardClass.hpp>

#include <mock/MockApplication.hpp>
#include <mock/MockLowLevelRecord.hpp>
#include <mock/MockDatabase.hpp>

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

class MyDatabase : public mock::MockDatabase {
public:
   MyDatabase (app::Application& app) : mock::MockDatabase (app) {;}

   void add (const mock::MockLowLevelRecord& record) noexcept {
      m_container [record.m_id] = record;
   }

private:
   mock::MockLowLevelContainer m_container;

   dbms::Statement* allocateStatement (const char* name, const std::string& expression, const dbms::ActionOnError::_v actionOnError)
      throw (adt::RuntimeException)
   {
      if (expression == "read" || expression == "READ")
         return new MyReadStatement (std::ref (*this), name, expression.c_str(), actionOnError);

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

   void releaseDependences () noexcept {;}
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


private:
   dbms::ResultCode do_apply (dbms::GuardStatement& statement, GuardClass& _class, Object& object) throw (adt::RuntimeException, dbms::DatabaseException);
   bool hasToRefresh (GuardClass& _class, const Object& object) throw (adt::RuntimeException, dbms::DatabaseException) { return false; }
   bool isInputValue (const int columnNumber) const noexcept { return columnNumber == 0; }
   bool isPrimaryKeyComponent (const int columnNumber) const noexcept { return columnNumber == 0; }
   bool isOutputValue (const int columnNumber) const noexcept { return columnNumber >= 1; }
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

   return ResultCode (getDatabase(), (m_isValid == false) ? MyDatabase::NotFound: MyDatabase::Successful);
}

bool test_persistence::MyReadStatement::do_fetch () throw (adt::RuntimeException, DatabaseException)
{
   if (m_isValid == true) {
      m_isValid = false;
      return true;
   }

   return false;
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

using namespace wepa;
using namespace wepa::mock;

BOOST_AUTO_TEST_CASE (persistence_storage_readonly)
{
   MockApplication application ("persistence_storage_readonly");

   test_persistence::MyDatabase database (application);

   application.disableTermination();

   MockLowLevelRecord record;
   for (int ii = 0; ii < 10; ++ ii) {
      record.m_id = ii;
      record.m_name = adt::StreamString ("the name ") << ii;
      database.add (record);
   }

   dbms::Connection* conn0 = database.createConnection("0", "0", "0");
   dbms::Statement* stReader = database.createStatement("the_read", "read");

   std::thread tr (std::ref (application));
   usleep (500);

   BOOST_REQUIRE_EQUAL (application.isRunning(), true);
   BOOST_REQUIRE_EQUAL (database.isRunning(), true);

   persistence::Repository repository ("persistence_storage_readonly");
   persistence::Storage* ii = repository.createStorage(0, "storage_readonly", persistence::Storage::AccessMode::ReadOnly);

   test_persistence::MockCustomerClass _class;

   BOOST_REQUIRE_NE (ii, (void*) 0);

   test_persistence::MockCustomerLoader myLoader;

   persistence::GuardClass guardCustomer (_class);

   BOOST_REQUIRE_NO_THROW(myLoader.setMember(guardCustomer, 0, 6));

   try{
      myLoader.initialize(_class, stReader);
      persistence::Object& object = ii->load(*conn0, guardCustomer, myLoader);
   }
   catch (adt::Exception& ex) {
      BOOST_REQUIRE_EQUAL (std::string ("no error"), ex.what ());
      std::cout << ex.what() << std::endl;
   }
}

