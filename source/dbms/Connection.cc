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
#include <wepa/config/defines.hpp>

#include <wepa/adt/DelayMeter.hpp>
#include <wepa/adt/Microsecond.hpp>

#include <wepa/logger/TraceMethod.hpp>

#include <wepa/xml/Node.hpp>
#include <wepa/xml/Attribute.hpp>

#include <wepa/dbms/Connection.hpp>
#include <wepa/dbms/Database.hpp>
#include <wepa/dbms/Statement.hpp>

using namespace std;
using namespace wepa;

//-----------------------------------------------------------------------------------------------------------
// (1) Si no tiene variables de salida => consideramos que es un update, insert o delete.
//-----------------------------------------------------------------------------------------------------------
dbms::ResultCode dbms::Connection::execute (Statement* statement)
   throw (adt::RuntimeException, dbms::DatabaseException)
{
   if (statement == NULL) {
      WEPA_THROW_EXCEPTION(asString () << " | Can not execute a NULL statement");
   }

   LOG_THIS_METHOD();

   adt::DelayMeter <adt::Microsecond> delay;

   if (statement->m_prepared == false) {
      statement->prepare (this);
      statement->m_prepared = true;
   }

   LOG_DEBUG("Using " << asString () << " to run " << statement->asString());

   if (statement->requiresCommit () == true && m_rollbackPending == true) {      // (1)
      WEPA_THROW_EXCEPTION(asString () << " | This connection must execute a previous ROLLBACK's");
   }

   ResultCode result;
   bool stop = false;

   while (stop == false) {
      result = statement->execute (this);

      if (result.lostConnection () == false)
         break;

      LOG_CRITICAL (asString () << " | " << statement->asString () << " | " << result.asString ());

      m_dbmsDatabase.breakConnection (*this);
      stop = true;
   }

   statement->measureTiming (delay);

   if (result.successful () == false && result.notFound () == false) {
      LOG_ERROR (asString () << " | " << statement->asString () << " | " << result.asString ());
   }

   if (statement->requiresCommit () == true) {  // (1)
      if (result.successful () == false) {
         if (statement->actionOnError() == ActionOnError::Rollback) {
            m_rollbackPending = true;
            WEPA_THROW_NAME_DB_EXCEPTION(statement->getName (), result);
         }
      }
      else {
         m_commitPending ++;
         if (m_maxCommitPending > 0 && m_commitPending > m_maxCommitPending)  {
            commit ();
            m_commitPending = 0;
            m_rollbackPending = false;
         }
      }
   }

   return result;
}

void dbms::Connection::commit () 
   throw (adt::RuntimeException, dbms::DatabaseException)
{
   LOG_INFO(asString ());
   
   if (isAvailable () == false) {
      WEPA_THROW_EXCEPTION(asString () << " | Connection is not available");
   }   
   
   do_commit ();
   m_commitPending = 0;
   m_rollbackPending = false;
}

void dbms::Connection::rollback () 
   throw ()
{
   LOG_WARN (asString ());

   if (isAvailable () == false) {
      LOG_WARN (asString () << " | Connection is not available");
   }

   do_rollback ();
   m_commitPending = 0;
   m_rollbackPending = false;
}

void dbms::Connection::lock ()
   throw (adt::RuntimeException)
{
   if (isAvailable () == false) {
      WEPA_THROW_EXCEPTION(asString () << " | Connection is not available");
   }   
   
   m_mutex.lock();

   if (m_lockingCounter ++ == 0) {
      m_accesingCounter ++;
      m_commitPending = 0;
      m_rollbackPending = false;
      try {
         if (do_beginTransaction () == true)
            m_commitPending = 1;
      }
      catch (dbms::DatabaseException& edb) {
         WEPA_THROW_EXCEPTION(edb.what());
      }
   }

   LOG_DEBUG (asString ());
}

void dbms::Connection::unlock ()
   throw ()
{
   LOG_DEBUG (asString ());

   if (-- m_lockingCounter <= 0) {
      m_lockingCounter = 0;
      try {
         if (m_rollbackPending == true)
            rollback ();
         else if (m_commitPending > 0)
            commit ();
      }
      catch (adt::Exception& ex) {
         logger::Logger::write (ex);
      }
   }

   m_mutex.unlock();
}

adt::StreamString dbms::Connection::asString () const
   throw ()
{
   adt::StreamString result ("dbms::Connection { ");
   result += balance::Resource::asString ();
   result << " | Database: " << m_dbmsDatabase.getName();
   result << " | User: " << m_user;
   result << " | LockingCounter: " << m_lockingCounter << " | password: ****";
   result << " | CommitPending: " << m_commitPending;
   result << " | RollbackPending: " << m_rollbackPending;
   return result += " }";
}

xml::Node& dbms::Connection::asXML (xml::Node& parent) const
   throw ()
{
   xml::Node& result = parent.createChild ("dbms.Connection");

   balance::Resource::asXML (result);

   result.createAttribute ("User", m_user);
   result.createAttribute ("AccessingCounter", m_accesingCounter);
   result.createAttribute ("LockingCounter", m_lockingCounter);
   result.createAttribute ("CommitPending", m_commitPending);
   result.createAttribute ("RollbackPending", m_rollbackPending);

   return result;
}

