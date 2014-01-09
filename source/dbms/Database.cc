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
#include <stdlib.h>
#include <locale.h>

#include <string>
#include <algorithm>

#include <wepa/config/defines.hpp>

#include <wepa/logger/TraceMethod.hpp>
#include <wepa/logger/Logger.hpp>

#include <wepa/xml/Node.hpp>
#include <wepa/xml/Attribute.hpp>

#include <wepa/dbms/Database.hpp>
#include <wepa/dbms/Statement.hpp>
#include <wepa/dbms/Connection.hpp>

#include <wepa/dbms/FailRecoveryHandler.hpp>
#include <wepa/dbms/StatementTranslator.hpp>
#include <wepa/dbms/SCCS.hpp>

using namespace std;
using namespace wepa;

//typedef Guard <dbms::Database> MyGuard;

dbms::Database::Database (app::Application& app, const char* className, const char* dbmsName) :
   app::EngineIf (app, className),
   m_name ((dbmsName == NULL) ? "local": dbmsName),
   m_type ((dbmsName == NULL) ? Type::Local: Type::Remote),
   m_failRecoveryHandler (NULL),
   m_statementTranslator (NULL)
{
   dbms::SCCS::activate ();
}

dbms::Database::~Database ()
{
   stop ();
}

void dbms::Database::do_initialize ()
   throw (adt::RuntimeException)
{
   LOG_THIS_METHOD();

   int counter (0);
   bool error = false;

   for (connection_iterator iic = connection_begin (), maxiic = connection_end (); iic != maxiic; iic ++) {
      try {
         connection (iic).open ();
         counter ++;
      }
      catch (adt::Exception& ex) {
         logger::Logger::write(ex);
         error = true;
      }
   }

   if (counter == 0 && error == true) {
      WEPA_THROW_EXCEPTION(asString () << " | There is not connection available");
   }

   LOG_INFO(asString () << " | It is initialized");
}

void dbms::Database::do_stop ()
   throw ()
{
   LOG_THIS_METHOD();

   try {
      for (connection_iterator iic = connection_begin (), maxiic = connection_end (); iic != maxiic; iic ++) {
         Connection& _connection = connection (iic);
         _connection.close ();
      }

      m_connections.clear ();
   }
   catch (adt::Exception& ex) {
      logger::Logger::write(ex);
      m_connections.clear ();
   }
}

dbms::Connection* dbms::Database::createConnection (const char* name, const char* user, const char* password)
   throw (adt::RuntimeException, dbms::DatabaseException)
{
   logger::TraceMethod ttmm (logger::Level::Local7, WEPA_FILE_LOCATION);

   LOG_DEBUG ("Name=" << name << " | User=" << user);

   if (m_connections.size () >= MaxConnection) {
      WEPA_THROW_EXCEPTION(asString () << " Can not create more than " << MaxConnection);
   }

   for (connection_iterator ii = connection_begin (), maxii = connection_end (); ii != maxii; ii ++) {
      if (connection (ii).getName () == name) {
         WEPA_THROW_EXCEPTION(asString () << " | Connection=" << name << " is already defined");
      }
   }

   string strname (name);

   Connection* result = allocateConnection (strname, user, password);

   if (result == NULL) {
      WEPA_THROW_EXCEPTION(asString () << " could not create connection");
   }

   LOG_DEBUG (result->asString ());

   if (this->isRunning () == true) {
      try {
         result->open ();
         m_connections.push_back (result);
      }
      catch (adt::Exception& ex) {
         logger::Logger::write(ex);
         delete result;
         throw;
      }
   }
   else
      m_connections.push_back (result);

   return result;
}

dbms::Connection& dbms::Database::findConnection (const char* name)
   throw (adt::RuntimeException)
{
   logger::TraceMethod ttmm (logger::Level::Local7, WEPA_FILE_LOCATION);

   LOG_DEBUG ("Name=" << name);

   Connection* result = NULL;

   for (connection_iterator ii = connection_begin (), maxii = connection_end (); ii != maxii; ii ++) {
      if (connection_ptr (ii)->getName () == name) {
         result = connection_ptr (ii);
         break;
      }
   }

   if (result == NULL) {
      WEPA_THROW_EXCEPTION(asString () << " | Connection='" << name << "' is not defined");
   }

   LOG_DEBUG (result->asString ());

   return std::ref (*result);
}

dbms::Statement* dbms::Database::createStatement (const char* name, const char* expression, const ActionOnError::_v actionOnError)
   throw (adt::RuntimeException)
{
   logger::TraceMethod ttmm (logger::Level::Local7, WEPA_FILE_LOCATION);

   LOG_DEBUG ("Name=" << name);

   for (statement_iterator ii = statement_begin(), maxii = statement_end(); ii != maxii; ++ ii) {
      if (statement(ii).getName () == name)
         WEPA_THROW_EXCEPTION("Sentence '" << name << "' is already defined");
   }

   if (m_statementTranslator != NULL)
      expression = m_statementTranslator->apply (expression);

   Statement* result = allocateStatement (name, expression, actionOnError);

   if (result == NULL) {
      WEPA_THROW_EXCEPTION(asString () << " could not create connection");
   }

   LOG_DEBUG(result->asString ());

   m_statements.push_back (result);

   return result;
}

dbms::Statement& dbms::Database::findStatement (const char* name)
   throw (adt::RuntimeException)
{
   logger::TraceMethod ttmm (logger::Level::Local7, WEPA_FILE_LOCATION);

   Statement* result (NULL);

   LOG_DEBUG ("Name=" << name);

   for (statement_iterator ii = statement_begin (), maxii = statement_end (); ii != maxii; ii ++) {
      if (statement_ptr (ii)->getName () == name) {
         result = statement_ptr (ii);
         break;
      }
   }

   if (result == NULL)
      WEPA_THROW_EXCEPTION("Statement '" << name << "' was not found");

   LOG_DEBUG (result->asString ());

   return std::ref (*result);
}

void dbms::Database::breakConnection (dbms::Connection& connection)
   throw (adt::RuntimeException)
{
   LOG_WARN(connection.asString ());

   try {
      connection.close ();
      connection.open ();

      LOG_WARN(connection.asString ());
   }
   catch (DatabaseException& edbms) {
      logger::Logger::write (edbms);
      if (m_failRecoveryHandler != NULL)
         m_failRecoveryHandler->apply (connection);
   }
}

adt::StreamString dbms::Database::asString () const
   throw ()
{
   adt::StreamString result ("dbms::Database { ");

   result << app::EngineIf::asString ();

   if (m_type == Type::Local)
      result += " | Type: Local";
   else {
      result += " | Type: Remote | Name: ";
      result += m_name;
   }

   if (m_statementTranslator)
      result << " | StatementTranslator=" << m_statementTranslator->getName ();

   return result += " }";
}

xml::Node& dbms::Database::asXML (xml::Node& parent) const
   throw ()
{
   xml::Node& result = parent.createChild ("dbms.Database");

   app::EngineIf::asXML (result);

   result.createAttribute ("Type", (m_type == Type::Local) ? "Local": "Remote");

   if (m_type != Type::Local)
      result.createAttribute ("Name", m_name);

   if (m_statementTranslator != NULL)
      result.createAttribute ("Translator", m_statementTranslator->getName ());

   xml::Node& connections = result. createChild ("Connections");
   for (const_connection_iterator ii = connection_begin (), maxii = connection_end (); ii != maxii; ii ++)
      connection (ii).asXML (connections);

   xml::Node& statements = result. createChild ("Statements");
   for (const_statement_iterator ii = statement_begin (), maxii = statement_end (); ii != maxii; ii ++)
      statement (ii).asXML (statements);

   return result;
}

