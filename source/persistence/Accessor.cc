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

#include <wepa/adt/AsString.hpp>

#include <wepa/persistence/Accessor.hpp>
#include <wepa/persistence/Class.hpp>

#include <wepa/logger/Logger.hpp>
#include <wepa/logger/TraceMethod.hpp>

#include <wepa/dbms/Connection.hpp>
#include <wepa/dbms/GuardConnection.hpp>
#include <wepa/dbms/Statement.hpp>
#include <wepa/dbms/GuardStatement.hpp>
#include <wepa/dbms/datatype/Abstract.hpp>
#include <wepa/dbms/datatype/Abstract.hpp>

#include <wepa/dbms/datatype/Date.hpp>
#include <wepa/dbms/datatype/Float.hpp>
#include <wepa/dbms/datatype/Integer.hpp>
#include <wepa/dbms/datatype/LongBlock.hpp>
#include <wepa/dbms/datatype/ShortBlock.hpp>
#include <wepa/dbms/datatype/String.hpp>

#include <wepa/persistence/GuardClass.hpp>

using namespace wepa;

void persistence::Accessor::initialize (GuardClass& _class, dbms::Statement* statement)
   throw (adt::RuntimeException)
{
   LOG_THIS_METHOD();

   if (m_statement != NULL) {
      WEPA_THROW_EXCEPTION(asString () << " | Already initialized");
   }

   m_statement = statement;

   if (m_statement == NULL) {
      m_inputValues.clear ();
      m_outputValues.clear ();
      m_primaryKey.clear ();
      WEPA_THROW_EXCEPTION(asString () << " | Statement can not be null");
   }

   if (m_inputValues.size() != 0 || m_outputValues.size() != 0) {
      WEPA_THROW_EXCEPTION(asString () << " | " << m_statement->asString () << " | Accessor already initialized");
   }

   if ( _class->member_size() == 0) {
      WEPA_THROW_EXCEPTION(_class->asString () << " | This is a empty class");
   }

   int pos = 0;
   dbms::datatype::Abstract* dataType = NULL;

   for (int ii = 0, maxii = _class->member_size(); ii != maxii; ++ ii) {
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

   LOG_DEBUG (_class->asString () << " | " << m_statement->asString ());
}

dbms::ResultCode persistence::Accessor::apply (dbms::Connection& connection, GuardClass& _class, Object& object)
   throw (adt::RuntimeException, dbms::DatabaseException)
{
   LOG_THIS_METHOD();

   if (m_statement == NULL) {
      WEPA_THROW_EXCEPTION(asString () << " | Statement can not be null");
   }

   if (m_inputValues.size() == 0 && m_outputValues.size() == 0) {
      WEPA_THROW_EXCEPTION(asString () << " | " << m_statement->asString () << " | Accessor was not initialized");
   }

   ++ m_applyCounter;

   LOG_DEBUG (getName () << " | " << m_primaryKey);

   dbms::GuardConnection gConnection (connection);

   dbms::GuardStatement gStatement (gConnection, *m_statement);

   dbms::ResultCode result = do_apply(gStatement, _class, object);

   LOG_DEBUG (getName () << " | " << result);

   return result;
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

void persistence::Accessor::setMember (GuardClass& _class, const int columnNumber, const int value)
   throw (adt::RuntimeException)
{
   if (isOutputValue(columnNumber) == true) {
      WEPA_THROW_EXCEPTION(asString () << " | Can not use " << __func__ << " over an output value");
   }

   dbms::datatype::Abstract& abstract =_class.getMember(columnNumber);
   wepa_datatype_downcast(dbms::datatype::Integer, abstract).setValue (value);
   LOG_DEBUG (getName () << " | ColumnNumber=" << columnNumber << " | Value=" << value);
}

void persistence::Accessor::setMember (GuardClass& _class, const int columnNumber, const char* value)
   throw (adt::RuntimeException)
{
   if (isOutputValue(columnNumber) == true) {
      WEPA_THROW_EXCEPTION(asString () << " | Can not use " << __func__ << " over an output value");
   }

   dbms::datatype::Abstract& abstract =_class.getMember(columnNumber);
   wepa_datatype_downcast(dbms::datatype::String, abstract).setValue (value);
   LOG_DEBUG (getName () << " | ColumnNumber=" << columnNumber << " | Value='" << value << "'");
}

void persistence::Accessor::setMember (GuardClass& _class, const int columnNumber, const float value)
   throw (adt::RuntimeException)
{
   if (isOutputValue(columnNumber) == true) {
      WEPA_THROW_EXCEPTION(asString () << " | Can not use " << __func__ << " over an output value");
   }

   dbms::datatype::Abstract& abstract =_class.getMember(columnNumber);
   wepa_datatype_downcast(dbms::datatype::Float, abstract).setValue (value);
   LOG_DEBUG (getName () << " | ColumnNumber=" << columnNumber << " | Value=" << value);
}

void persistence::Accessor::setMember (GuardClass& _class, const int columnNumber, const adt::DataBlock& value)
   throw (adt::RuntimeException)
{
   if (isOutputValue(columnNumber) == true) {
      WEPA_THROW_EXCEPTION(asString () << " | Can not use " << __func__ << " over an output value");
   }

   dbms::datatype::Abstract& abstract =_class.getMember(columnNumber);

   if (abstract.getType() == dbms::datatype::Abstract::Datatype::LongBlock) {
      wepa_datatype_downcast (dbms::datatype::LongBlock, abstract).setValue (value);
   }
   else {
      wepa_datatype_downcast (dbms::datatype::ShortBlock, abstract).setValue (value);
   }

   LOG_DEBUG (getName () << " | ColumnNumber=" << columnNumber << " | Value=" << adt::AsString::apply (value));
}

void persistence::Accessor::setMember (GuardClass& _class, const int columnNumber, const adt::Second& value)
   throw (adt::RuntimeException)
{
   if (isOutputValue(columnNumber) == true) {
      WEPA_THROW_EXCEPTION(asString () << " | Can not use " << __func__ << " over an output value");
   }

   dbms::datatype::Abstract& abstract =_class.getMember(columnNumber);
   wepa_datatype_downcast(dbms::datatype::Date, abstract).setValue (value);
   LOG_DEBUG (getName () << " | ColumnNumber=" << columnNumber << " | Value=" << value.asString ());
}

int persistence::Accessor::readInteger (GuardClass& _class, const int columnNumber) const
   throw (adt::RuntimeException)
{
   dbms::datatype::Abstract& abstract =_class.getMember(columnNumber);
   return wepa_datatype_downcast(dbms::datatype::Integer, abstract).getValue();
}

const char* persistence::Accessor::readCString (GuardClass& _class, const int columnNumber) const
   throw (adt::RuntimeException)
{
   dbms::datatype::Abstract& abstract =_class.getMember(columnNumber);
   return wepa_datatype_downcast(dbms::datatype::String, abstract).getValue();
}

float persistence::Accessor::readFloat (GuardClass& _class, const int columnNumber) const
   throw (adt::RuntimeException)
{
   dbms::datatype::Abstract& abstract =_class.getMember(columnNumber);
   return wepa_datatype_downcast(dbms::datatype::Float, abstract).getValue();
}

const adt::DataBlock& persistence::Accessor::readDataBlock (GuardClass& _class, const int columnNumber) const
   throw (adt::RuntimeException)
{
   dbms::datatype::Abstract& abstract =_class.getMember(columnNumber);


   if (abstract.getType() == dbms::datatype::Abstract::Datatype::LongBlock) {
      return wepa_datatype_downcast(dbms::datatype::LongBlock, abstract).getValue();
   }
   else {
      return wepa_datatype_downcast(dbms::datatype::ShortBlock, abstract).getValue();
   }
}

const adt::Second& persistence::Accessor::readDate (GuardClass& _class, const int columnNumber) const
   throw (adt::RuntimeException)
{
   dbms::datatype::Abstract& abstract =_class.getMember(columnNumber);
   return wepa_datatype_downcast(dbms::datatype::Date, abstract).getValue();
}
