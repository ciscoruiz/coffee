// COFFEE - COmpany eFFEEctive Platform
//
//(c) Copyright 2018 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/coffee
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#include <coffee/config/defines.hpp>

#include <coffee/adt/DelayMeter.hpp>
#include <coffee/adt/Microsecond.hpp>

#include <coffee/logger/TraceMethod.hpp>

#include <coffee/xml/Node.hpp>
#include <coffee/xml/Attribute.hpp>

#include <coffee/dbms/Connection.hpp>
#include <coffee/dbms/Database.hpp>
#include <coffee/dbms/Statement.hpp>

using namespace std;
using namespace coffee;

//-----------------------------------------------------------------------------------------------------------
//(1) Si no tiene variables de salida => consideramos que es un update, insert o delete.
//-----------------------------------------------------------------------------------------------------------
dbms::ResultCode dbms::Connection::execute(std::shared_ptr<Statement>& statement)
   throw(adt::RuntimeException, dbms::DatabaseException)
{
   LOG_THIS_METHOD();

   adt::DelayMeter <adt::Microsecond> delay;

   LOG_DEBUG("Using " << asString() << " to run " << statement->asString());

   if(statement->requiresCommit() == true && m_rollbackPending == true) {      //(1)
      COFFEE_THROW_EXCEPTION(asString() << " | This connection must execute a previous ROLLBACK's");
   }

   ResultCode result = statement->execute(*this);

   if(result.lostConnection() == true) {
      LOG_CRITICAL("Detected lost connection " << asString() << " while running '" << statement->getName()  << "' | " << result);
      recover();
      COFFEE_THROW_NAME_DB_EXCEPTION(statement->getName(), result);
   }

   statement->measureTiming(delay);

   if(result.successful() == false && result.notFound() == false) {
      LOG_ERROR(asString() << " | " << statement->asString() << " | " << result);
   }

   if(statement->requiresCommit() == false)
      return result;

   if(result.successful() == false) {
      if(statement->actionOnError() == ActionOnError::Rollback) {
         m_rollbackPending = true;
         COFFEE_THROW_NAME_DB_EXCEPTION(statement->getName(), result);
      }
   }
   else {
      m_commitPending ++;
      if(reachMaxCommitPending())  {
         commit();
      }
   }

   return result;
}

void dbms::Connection::commit()
   throw(adt::RuntimeException, dbms::DatabaseException)
{
   LOG_INFO(asString() << " | State before commit");
   
   if(isAvailable() == false) {
      COFFEE_THROW_EXCEPTION(asString() << " | Connection is not available");
   }   
   
   do_commit();
   m_commitPending = 0;
   m_rollbackPending = false;

   LOG_DEBUG(asString() << " | State after commit");
}

void dbms::Connection::rollback()
   noexcept
{
   LOG_WARN(asString() << " | State before rollback");

   if(isAvailable() == false) {
      LOG_WARN(asString() << " | Connection is not available");
   }

   do_rollback();
   m_commitPending = 0;
   m_rollbackPending = false;

   LOG_DEBUG(asString() << " | State after rollback");
}

void dbms::Connection::lock()
   throw(adt::RuntimeException)
{
   if(isAvailable() == false) {
      if(recover() == false) {
         COFFEE_THROW_EXCEPTION(asString() << " | Connection is not available");
      }
   }   
   
   m_mutex.lock();

   if(m_lockingCounter ++ == 0) {
      m_accesingCounter ++;
      m_commitPending = 0;
      m_rollbackPending = false;
      try {
         if(do_beginTransaction() == true)
            m_commitPending = 1;
      }
      catch(dbms::DatabaseException& edb) {
         COFFEE_THROW_EXCEPTION(edb.what());
      }
   }

   LOG_DEBUG(asString());
}

void dbms::Connection::unlock()
   noexcept
{
   LOG_DEBUG(asString());

   if(-- m_lockingCounter <= 0) {
      m_lockingCounter = 0;
      try {
         if(m_rollbackPending == true)
            rollback();
         else if(m_commitPending > 0)
            commit();
      }
      catch(adt::Exception& ex) {
         logger::Logger::write(ex);
      }
   }

   m_mutex.unlock();
}

bool dbms::Connection::recover()
   noexcept
{
   LOG_THIS_METHOD();

   bool result = false;

   try {
      close();
      open();
      result = true;
   }
   catch(DatabaseException& edbms) {
      logger::Logger::write(edbms);
      m_dbmsDatabase.notifyRecoveryFail(*this);
   }

   LOG_WARN(asString() << " | Result=" << result);

   return result;
}

adt::StreamString dbms::Connection::asString() const
   noexcept
{
   adt::StreamString result("dbms::Connection { ");
   result += balance::Resource::asString();
   result << " | Database=" << m_dbmsDatabase.getName();
   result << " | User=" << m_user;
   result << " | LockingCounter=" << m_lockingCounter << " | password=****";
   result << " | MaxCommitPending=" << m_maxCommitPending;
   result << " | CommitPending=" << m_commitPending;
   result << " | RollbackPending=" << m_rollbackPending;
   return result += " }";
}

xml::Node& dbms::Connection::asXML(xml::Node& parent) const
   noexcept
{
   xml::Node& result = parent.createChild("dbms.Connection");

   balance::Resource::asXML(result);

   result.createAttribute("User", m_user);
   result.createAttribute("AccessingCounter", m_accesingCounter);
   result.createAttribute("LockingCounter", m_lockingCounter);
   result.createAttribute("MaxCommitPending", m_maxCommitPending);
   result.createAttribute("CommitPending", m_commitPending);
   result.createAttribute("RollbackPending", m_rollbackPending);

   return result;
}

