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
#include <wepa/logger/Logger.hpp>

#include <wepa/dbms/datatype/Integer.hpp>
#include <wepa/dbms/datatype/String.hpp>
#include <wepa/dbms/datatype/Float.hpp>
#include <wepa/dbms/datatype/Date.hpp>

#include "PersistenceMocks.hpp"

using namespace wepa;

dbms::ResultCode test_persistence::MyReadStatement::do_execute(dbms::Connection& connection)
   throw(adt::RuntimeException, dbms::DatabaseException)
{
   m_isValid = false;

   mock::MockConnection& _connection(static_cast <mock::MockConnection&>(connection));

   const int searchedId = wepa_datatype_downcast(dbms::datatype::Integer, getInputData(0))->getValue();

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

      wepa_datatype_downcast(dbms::datatype::String, getOutputData(0))->setValue(m_selection.m_name);

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

   record.m_id =(wepa_datatype_downcast(dbms::datatype::Integer, getInputData(0))->getValue());

   if(record.m_id == IdToThrowDbException) {
      result.initialize(MyDatabase::NotFound, NULL);
      return result;
   }

   if(opCode != mock::MockConnection::Delete) {
      record.m_name = wepa_datatype_downcast(dbms::datatype::String, getInputData(1))->getValue();
   }

   LOG_DEBUG("ID = " << record.m_id);

   static_cast <mock::MockConnection&>(connection).addOperation(opCode, record);

   return result;
}

dbms::ResultCode test_persistence::MockCustomerLoader::apply(dbms::GuardStatement& statement, TheObject& object)
   throw(adt::RuntimeException, dbms::DatabaseException)
{
   const int id = object->getPrimaryKey()->getInteger("id");

   wepa_datatype_downcast(dbms::datatype::Integer, statement.getInputData(0))->setValue(id);

   dbms::ResultCode result = statement.execute();

   if(result.successful() == true) {
      if(statement.fetch()) {
         CustomerObjectWrapper customer(object);
         customer.setId(id);
         customer.setName(wepa_datatype_downcast(dbms::datatype::String, statement.getOutputData(0))->getValue());
      }
   }

   return result;
}

dbms::ResultCode test_persistence::MockCustomerRecorder::apply(dbms::GuardStatement& statement) const
   throw(adt::RuntimeException, dbms::DatabaseException)
{
   CustomerObjectWrapper customer(getObject());

   wepa_datatype_downcast(dbms::datatype::Integer, statement.getInputData(0))->setValue(customer.getId());
   wepa_datatype_downcast(dbms::datatype::String, statement.getInputData(1))->setValue(customer.getName());

   return statement.execute();
}

dbms::ResultCode test_persistence::MockCustomerEraser::apply(dbms::GuardStatement& statement) const
   throw(adt::RuntimeException, dbms::DatabaseException)
{
   auto primaryKey = getPrimaryKey();

   const int id = primaryKey->getInteger("id");
   wepa_datatype_downcast(dbms::datatype::Integer, statement.getInputData(0))->setValue(id);

   return statement.execute();
}
