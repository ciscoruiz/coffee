// WEPA - Write Excellent Professional Applications
//
//(c) Copyright 2013 Francisco Ruiz Rayo
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#include "MockDatabase.hpp"
#include "MockConnection.hpp"

#include <wepa/logger/Logger.hpp>
#include <wepa/logger/TraceMethod.hpp>

using namespace wepa;

mock::MockConnection::MockConnection (MockDatabase& database, const std::string& name, const char* user, const char* password) :
   dbms::Connection (database, name, user, password),
   m_refContainer(database.m_container),
   m_container(nullptr)
{
   m_commitCounter = m_rollbackCounter = m_openCounter = m_closeCounter = 0;
   m_avoidRecovery = false;
}

void mock::MockConnection::open()
   throw(dbms::DatabaseException)
{
   m_operations.clear();

   if(m_avoidRecovery == true) {
      WEPA_THROW_DB_EXCEPTION(dbms::ResultCode(getDatabase(), MockDatabase::LostConnection));
   }

   ++ m_openCounter;
   m_container = &m_refContainer;
   LOG_DEBUG("OpenCounter=" << m_openCounter);
}

void mock::MockConnection::close() noexcept
{
   ++ m_closeCounter;
   LOG_DEBUG("CloseCounter=" << m_closeCounter);
}

void mock::MockConnection::do_commit() noexcept
{
   LOG_THIS_METHOD();

   mock::MockLowLevelContainer& container(*m_container);

   m_commitCounter ++;

   LOG_DEBUG("Commiting " << operation_size() << " changes");

   for(Operation& operation : m_operations) {
      switch(operation.first) {
      case Write:
         LOG_DEBUG("Writing " << operation.second.m_id);
         container[operation.second.m_id] = operation.second;
         break;
      case Delete:
         {
            auto ii = container.find(operation.second.m_id);
            if(ii != container.end()) {
               LOG_DEBUG("Deleting " << operation.second.m_id);
               container.erase(ii);
            }
         }
         break;
      }
   }

   m_operations.clear();
}

void mock::MockConnection::do_rollback()
   noexcept
{
   LOG_THIS_METHOD();
   m_rollbackCounter ++;
   m_operations.clear();
}

mock::MockLowLevelContainer& mock::MockConnection::getContainer()
   throw(adt::RuntimeException)
{
   if(m_container == NULL)
      WEPA_THROW_EXCEPTION("Connection " << getName() << " is not available");

   return std::ref(*m_container);
}
