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


#ifndef TEST_PERSISTENCE_PERSISTENCEMOCKS_HPP_
#define TEST_PERSISTENCE_PERSISTENCEMOCKS_HPP_

#include <coffee/dbms/Database.hpp>
#include <coffee/dbms/GuardConnection.hpp>
#include <coffee/dbms/GuardStatement.hpp>
#include <coffee/dbms/Statement.hpp>
#include <coffee/dbms/DatabaseException.hpp>

#include "../dbms.mock/MockLowLevelRecord.hpp"
#include "../dbms.mock/MockDatabase.hpp"

#include <coffee/persistence/Loader.hpp>
#include <coffee/persistence/Eraser.hpp>
#include <coffee/persistence/Recorder.hpp>
#include <coffee/persistence/Accessor.hpp>
#include <coffee/persistence/Object.hpp>

namespace coffee {

namespace test_persistence {

static const int IdToThrowDbException = 0;

using namespace coffee::dbms;
using namespace coffee::mock;

class MyReadStatement : public dbms::Statement {
public:
   MyReadStatement(const dbms::Database& database, const char* name, const char* expression, const StatementParameters& parameters) :
      dbms::Statement(database, name, expression, parameters),
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

   void do_prepare(dbms::Connection& connection) throw(adt::RuntimeException, dbms::DatabaseException) {;}
   dbms::ResultCode do_execute(dbms::Connection& connection) throw(adt::RuntimeException, dbms::DatabaseException);
   bool do_fetch() throw(adt::RuntimeException, dbms::DatabaseException);
};

class MyWriteStatement : public dbms::Statement {
public:
   MyWriteStatement(const dbms::Database& database, const char* name, const char* expression, const StatementParameters& parameters) :
      dbms::Statement(database, name, expression, parameters)
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

   void do_prepare(dbms::Connection& connection) throw(adt::RuntimeException, dbms::DatabaseException) {;}
   dbms::ResultCode do_execute(dbms::Connection& connection) throw(adt::RuntimeException, dbms::DatabaseException);
   bool do_fetch() throw(adt::RuntimeException, dbms::DatabaseException) { return false; }
};

class MyDatabase : public mock::MockDatabase {
public:
   static const int PreloadRegisterCounter;

   static std::shared_ptr<MyDatabase> instantiate(app::Application& application)
      throw(adt::RuntimeException)
   {
      std::shared_ptr<MyDatabase> result(new MyDatabase(application));
      application.attach(result);
      return result;
   }

private:
   explicit MyDatabase(app::Application& app) : mock::MockDatabase(app) { fillup(); }

   void fillup() {
      mock::MockLowLevelRecord record;
      for(int ii = 0; ii < PreloadRegisterCounter; ++ ii) {
         record.m_id = ii;
         record.m_name = adt::StreamString("the name ") << ii;
         add(record);
      }
   }
   std::shared_ptr<Statement> allocateStatement(const char* name, const std::string& expression, const StatementParameters& parameters)
      throw(adt::RuntimeException)
   {
      std::shared_ptr<Statement> result;

      if(expression == "read" || expression == "READ")
         result = std::make_shared<MyReadStatement>(*this, name, expression.c_str(), parameters);

      if(expression == "write" || expression == "delete")
         result = std::make_shared<MyWriteStatement>(*this, name, expression.c_str(), parameters);

      if(!result)
         COFFEE_THROW_EXCEPTION(name << " invalid statement");

      return result;
   }

   void do_stop() throw(adt::RuntimeException) {;}
};

class CustomerObjectWrapper {
public:
   explicit CustomerObjectWrapper(std::shared_ptr<persistence::Object> object) : m_object(object) { }

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
