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

#include "MockDatabase.hpp"
#include "MockConnection.hpp"

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TraceMethod.hpp>

using namespace coffee;

mock::MockConnection::MockConnection (MockDatabase& database, const std::string& name, const dbms::ConnectionParameters& parameters) :
   dbms::Connection (database, name, parameters),
   m_container(database.m_container),
   m_isOpen(false)
{
   m_commitCounter = m_rollbackCounter = m_openCounter = m_closeCounter = 0;
   m_avoidRecovery = false;
}

void mock::MockConnection::open()
   throw(dbms::DatabaseException)
{
   m_operations.clear();

   if(m_avoidRecovery == true) {
      COFFEE_THROW_DB_EXCEPTION(dbms::ResultCode(getDatabase(), MockDatabase::LostConnection));
   }

   ++ m_openCounter;
   m_isOpen = true;
   LOG_DEBUG("OpenCounter=" << m_openCounter);
}

void mock::MockConnection::close() noexcept
{
   ++ m_closeCounter;
   m_isOpen = false;
   LOG_DEBUG("CloseCounter=" << m_closeCounter);
}

void mock::MockConnection::do_commit()
   throw (basis::RuntimeException, dbms::DatabaseException)
{
   LOG_THIS_METHOD();

   if (m_isOpen == false) {
      COFFEE_THROW_EXCEPTION(asString() << " is not open");
   }

   m_commitCounter ++;

   LOG_DEBUG("Committing " << operation_size() << " changes");

   for(Operation& operation : m_operations) {
      switch(operation.first) {
      case Write:
         LOG_DEBUG("Writing " << operation.second.m_id);
         m_container[operation.second.m_id] = operation.second;
         break;
      case Delete:
         {
            auto ii = m_container.find(operation.second.m_id);
            if(ii != m_container.end()) {
               LOG_DEBUG("Deleting " << operation.second.m_id);
               m_container.erase(ii);
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
   throw(basis::RuntimeException)
{
   if (m_isOpen == false) {
      COFFEE_THROW_EXCEPTION("Connection " << getName() << " is not available");
   }

   return std::ref(m_container);
}
