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

#include <functional>

#include <wepa/persistence/Accessor.hpp>
#include <wepa/persistence/Class.hpp>

#include <wepa/logger/Logger.hpp>

#include <wepa/dbms/Connection.hpp>
#include <wepa/dbms/GuardConnection.hpp>
#include <wepa/dbms/Statement.hpp>
#include <wepa/dbms/GuardStatement.hpp>
#include <wepa/dbms/datatype/Abstract.hpp>

using namespace wepa;

void persistence::Accessor::initialize (Class& _class, dbms::Statement* statement)
   throw (adt::RuntimeException)
{
   m_statement = statement;

   if (m_statement == NULL) {
      WEPA_THROW_EXCEPTION(asString () << " | Statement can not be null");
   }

   if (m_inputValues.size() != 0 ||  m_outputValues.size() != 0) {
      WEPA_THROW_EXCEPTION(asString () << " | " << m_statement->asString () << " | Accessor already initialized");
   }

   int pos = 0;
   dbms::datatype::Abstract* dataType = NULL;

   for (int ii = 0, maxii = _class.member_size(); ii != maxii; ++ ii) {
      if (this->isInputValue(ii) == true) {
         dbms::datatype::Abstract& member = _class.getMember(ii);

         m_inputValues.push_back(&member);
         m_statement->createBinderInput (member);

         if (this->isPrimaryKeyComponent(ii) == true) {
            m_primaryKey.addComponent(&member);
         }
      }

      if (this->isOutputValue(ii) == true) {
         dbms::datatype::Abstract& member = _class.getMember(ii);
         m_outputValues.push_back(&member);
         m_statement->createBinderOutput (member);
      }
   }

   LOG_DEBUG (getName () << " | " << m_primaryKey);

   LOG_DEBUG (_class << " | " << m_statement->asString ());
}

void persistence::Accessor::apply (dbms::Connection& connection, GuardClass& _class, Object& object)
   throw (adt::RuntimeException, dbms::DatabaseException)
{
   if (m_statement == NULL) {
      WEPA_THROW_EXCEPTION(asString () << " | Statement can not be null");
   }

   if (m_inputValues.size() == 0 && m_outputValues.size() == 0) {
      WEPA_THROW_EXCEPTION(asString () << " | " << m_statement->asString () << " | Accessor was not initialized");
   }

   dbms::GuardConnection gConnection (connection);

   dbms::GuardStatement gStatement (gConnection, *m_statement);

   // Data will be transfered by using 'binders' from statement to _class
   do_apply(gConnection, gStatement, _class, object);
}

const persistence::PrimaryKey& persistence::Accessor::getPrimaryKey () const
   throw (adt::RuntimeException)
{
   if (m_primaryKey.isDefined() == false) {
      WEPA_THROW_EXCEPTION(this->asString () << " | Primary key is not defined");
   }

   return std::ref (m_primaryKey);
}

dbms::Statement& persistence::Accessor::getStatement ()
   throw (adt::RuntimeException)
{
   if (m_statement == NULL) {
      WEPA_THROW_EXCEPTION("Accessor " << asString () << " does not have associated statement");
   }

   return std::ref (*m_statement);
}

