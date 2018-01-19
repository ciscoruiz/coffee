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

#ifndef TEST_PERSISTENCE_PERSISTENCEMOCKS_HPP_
#define TEST_PERSISTENCE_PERSISTENCEMOCKS_HPP_

#include <wepa/dbms/Database.hpp>
#include <wepa/dbms/GuardConnection.hpp>
#include <wepa/dbms/GuardStatement.hpp>
#include <wepa/dbms/Statement.hpp>
#include <wepa/dbms/DatabaseException.hpp>

#include "../dbms/MockLowLevelRecord.hpp"
#include "../dbms/MockDatabase.hpp"

#include <wepa/persistence/Loader.hpp>
#include <wepa/persistence/Eraser.hpp>
#include <wepa/persistence/Recorder.hpp>
#include <wepa/persistence/Accessor.hpp>
#include <wepa/persistence/Object.hpp>

namespace wepa {

namespace test_persistence {

static const int IdToThrowDbException = 0;

using namespace wepa::dbms;
using namespace wepa::mock;

class MyReadStatement : public dbms::Statement {
public:
   MyReadStatement(const dbms::Database& database, const char* name, const char* expression, const ActionOnError::_v actionOnError) :
      dbms::Statement(database, name, expression, actionOnError),
      m_isValid(false)
   {
      m_id = std::make_shared<datatype::Integer>("ID");
      createBinderInput(m_id);

      m_name = std::make_shared<datatype::String>("name", 64);
      createBinderOutput(m_name);
   }

   void setId(const int id) noexcept { m_id->setValue(id); }

private:
   bool m_isValid;
   std::shared_ptr<datatype::Integer> m_id;
   std::shared_ptr<datatype::String> m_name;
   mock::MockLowLevelRecord m_selection;

   void do_prepare() throw(adt::RuntimeException, dbms::DatabaseException) {;}
   dbms::ResultCode do_execute(dbms::Connection& connection) throw(adt::RuntimeException, dbms::DatabaseException);
   bool do_fetch() throw(adt::RuntimeException, dbms::DatabaseException);
};

class MyWriteStatement : public dbms::Statement {
public:
   MyWriteStatement(const dbms::Database& database, const char* name, const char* expression, const ActionOnError::_v actionOnError) :
      dbms::Statement(database, name, expression, actionOnError)
   {
      m_id = std::make_shared<datatype::Integer>("ID");
      m_name = std::make_shared<datatype::String>("name", 64);
      createBinderInput(m_id);
      createBinderInput(m_name);
   }

private:
   std::shared_ptr<datatype::Integer> m_id;
   std::shared_ptr<datatype::String> m_name;
   mock::MockLowLevelRecord m_selection;

   void do_prepare() throw(adt::RuntimeException, dbms::DatabaseException) {;}
   dbms::ResultCode do_execute(dbms::Connection& connection) throw(adt::RuntimeException, dbms::DatabaseException);
   bool do_fetch() throw(adt::RuntimeException, dbms::DatabaseException) { return false; }
};

class MyDatabase : public mock::MockDatabase {
public:
   static const int PreloadRegisterCounter;

   MyDatabase(app::Application& app) : mock::MockDatabase(app) { fillup(); }
   MyDatabase(const char* name) : mock::MockDatabase(name) { fillup(); }

private:
   void fillup() {
      mock::MockLowLevelRecord record;
      for(int ii = 0; ii < PreloadRegisterCounter; ++ ii) {
         record.m_id = ii;
         record.m_name = adt::StreamString("the name ") << ii;
         add(record);
      }
   }
   std::shared_ptr<Statement> allocateStatement(const char* name, const std::string& expression, const ActionOnError::_v actionOnError)
      throw(adt::RuntimeException)
   {
      std::shared_ptr<Statement> result;

      if(expression == "read" || expression == "READ")
         result = std::make_shared<MyReadStatement>(*this, name, expression.c_str(), actionOnError);

      if(expression == "write" || expression == "delete")
         result = std::make_shared<MyWriteStatement>(*this, name, expression.c_str(), actionOnError);

      if(!result)
         WEPA_THROW_EXCEPTION(name << " invalid statement");

      return result;
   }
};

class CustomerObjectWrapper {
public:
   CustomerObjectWrapper(std::shared_ptr<persistence::Object> object) : m_object(object) { }

   int getId() const throw(dbms::InvalidDataException) { return m_object->getPrimaryKey()->getInteger("id"); }
   std::string getName() const throw(dbms::InvalidDataException) { return m_object->getString("name"); }

   void setId(const int value) throw(dbms::InvalidDataException)  { m_object->getPrimaryKey()->setInteger("id", value); }
   void setName(const std::string& value) throw(dbms::InvalidDataException) { m_object->setString("name", value); }

private:
   std::shared_ptr<persistence::Object> m_object;
};

struct MockCustomerLoader : public persistence::Loader {
public:
   MockCustomerLoader(TheStatement& statement, ThePrimaryKey& primaryKey, const TheClass& clazz) :
      persistence::Loader("MockCustomerLoader", statement, primaryKey, clazz)
   {;}

private:
   dbms::ResultCode apply(dbms::GuardStatement& statement, TheObject& object) throw(adt::RuntimeException, dbms::DatabaseException);
   bool hasToRefresh (dbms::GuardStatement& statement, TheObject& object) throw (adt::RuntimeException, dbms::DatabaseException);
};

class MockCustomerRecorder : public persistence::Recorder {
public:
   MockCustomerRecorder(TheStatement& statement, const TheObject& object) : persistence::Recorder("MockCustomerRecorder", statement, object) {;}

private:
   dbms::ResultCode apply(dbms::GuardStatement& statement) throw(adt::RuntimeException, dbms::DatabaseException);
};

struct MockCustomerEraser : public persistence::Eraser {
public:
   MockCustomerEraser(TheStatement& statement, const ThePrimaryKey& primaryKey) : persistence::Eraser("MockCustomerEraser", statement, primaryKey) {;}

private:
   dbms::ResultCode apply(dbms::GuardStatement& statement) throw(adt::RuntimeException, dbms::DatabaseException);
};

}
}

#endif /* TEST_PERSISTENCE_PERSISTENCEMOCKS_HPP_ */
