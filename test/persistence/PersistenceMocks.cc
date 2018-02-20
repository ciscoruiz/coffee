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

#include <coffee/logger/Logger.hpp>

#include <coffee/dbms/datatype/Integer.hpp>
#include <coffee/dbms/datatype/String.hpp>
#include <coffee/dbms/datatype/Float.hpp>
#include <coffee/dbms/datatype/Date.hpp>

#include "PersistenceMocks.hpp"

using namespace coffee;

//static
const int test_persistence::MyDatabase::PreloadRegisterCounter = 100;

dbms::ResultCode test_persistence::MyReadStatement::do_execute(dbms::Connection& connection)
   throw(adt::RuntimeException, dbms::DatabaseException)
{
   m_isValid = false;

   mock::MockConnection& _connection(static_cast <mock::MockConnection&>(connection));

   const int searchedId = coffee_datatype_downcast(dbms::datatype::Integer, getInputData(0))->getValue();

   for(auto ii : _connection.getContainer()) {
      if(ii.second.m_id == searchedId) {
         m_selection = ii.second;
         m_isValid = true;
      }
   }

   ResultCode result(getDatabase(),(m_isValid == false) ? MyDatabase::NotFound: MyDatabase::Successful);

   LOG_DEBUG("Searching ID=" << searchedId << " | n-size=" << _connection.getContainer().size() << " | Result =" << result);

   return result;
}

bool test_persistence::MyReadStatement::do_fetch() throw(adt::RuntimeException, DatabaseException)
{
   if(m_isValid == true) {
      m_isValid = false;

      coffee_datatype_downcast(dbms::datatype::String, getOutputData(0))->setValue(m_selection.m_name);

      return true;
   }

   return false;
}

dbms::ResultCode test_persistence::MyWriteStatement::do_execute(dbms::Connection& connection)
   throw(adt::RuntimeException, dbms::DatabaseException)
{
   dbms::ResultCode result(getDatabase(), MyDatabase::Successful);

   mock::MockConnection::OpCode opCode =(getExpression() == "write") ? mock::MockConnection::Write: mock::MockConnection::Delete;

   mock::MockLowLevelRecord record;

   record.m_id =(coffee_datatype_downcast(dbms::datatype::Integer, getInputData(0))->getValue());

   if(record.m_id == IdToThrowDbException) {
      return dbms::ResultCode(getDatabase(),MyDatabase::NotFound);
   }

   if(opCode != mock::MockConnection::Delete) {
      record.m_name = coffee_datatype_downcast(dbms::datatype::String, getInputData(1))->getValue();
   }

   LOG_DEBUG("ID = " << record.m_id);

   static_cast <mock::MockConnection&>(connection).addOperation(opCode, record);

   return result;
}

dbms::ResultCode test_persistence::MockCustomerLoader::apply(dbms::GuardStatement& statement, TheObject& object)
   throw(adt::RuntimeException, dbms::DatabaseException)
{
   const int id = object->getPrimaryKey()->getInteger("id");

   coffee_datatype_downcast(dbms::datatype::Integer, statement.getInputData(0))->setValue(id);

   dbms::ResultCode result = statement.execute();

   if(result.successful() == true) {
      if(statement.fetch()) {
         CustomerObjectWrapper customer(object);
         customer.setId(id);
         customer.setName(coffee_datatype_downcast(dbms::datatype::String, statement.getOutputData(0))->getValue());
      }
   }

   return result;
}

bool test_persistence::MockCustomerLoader::hasToRefresh (dbms::GuardStatement& statement, TheObject& object) throw (adt::RuntimeException, dbms::DatabaseException)
{
   // It will only refresh odd id-numbers.
   return (object->getPrimaryKey()->getInteger("id") % 2) == 0;
}

dbms::ResultCode test_persistence::MockCustomerRecorder::apply(dbms::GuardStatement& statement)
   throw(adt::RuntimeException, dbms::DatabaseException)
{
   CustomerObjectWrapper customer(getObject());

   coffee_datatype_downcast(dbms::datatype::Integer, statement.getInputData(0))->setValue(customer.getId());
   coffee_datatype_downcast(dbms::datatype::String, statement.getInputData(1))->setValue(customer.getName());

   return statement.execute();
}

dbms::ResultCode test_persistence::MockCustomerEraser::apply(dbms::GuardStatement& statement)
   throw(adt::RuntimeException, dbms::DatabaseException)
{
   auto primaryKey = getPrimaryKey();

   const int id = primaryKey->getInteger("id");
   coffee_datatype_downcast(dbms::datatype::Integer, statement.getInputData(0))->setValue(id);

   return statement.execute();
}
