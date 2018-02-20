#include "MockDatabase.hpp"
#include "MockConnection.hpp"

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TraceMethod.hpp>

using namespace coffee;

mock::MockConnection::MockConnection (MockDatabase& database, const std::string& name, const char* user, const char* password) :
   dbms::Connection (database, name, user, password),
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
   throw (adt::RuntimeException, dbms::DatabaseException)
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
   throw(adt::RuntimeException)
{
   if (m_isOpen == false) {
      COFFEE_THROW_EXCEPTION("Connection " << getName() << " is not available");
   }

   return std::ref(m_container);
}
