#include <coffee/logger/Logger.hpp>

#include <coffee/dbms/Connection.hpp>
#include <coffee/dbms/GuardConnection.hpp>

using namespace coffee;

dbms::GuardConnection::GuardConnection(std::shared_ptr<Connection> connection)  throw (adt::RuntimeException) :
   m_connection (connection),
   m_countLinkedStatement (0)
{
   connection->lock();
}

dbms::GuardConnection::~GuardConnection()
{
   if (m_countLinkedStatement != 0) {
      LOG_CRITICAL(m_connection->asString() << " as freed before some of its linked GuardStatement");
   }

   m_connection->unlock();
}

int dbms::GuardConnection::setMaxCommitPending (const int maxCommitPending) noexcept
{
   return m_connection->setMaxCommitPending(maxCommitPending);
}

void dbms::GuardConnection::clearMaxCommitPending () noexcept
{
   m_connection->clearMaxCommitPending ();
}

dbms::ResultCode dbms::GuardConnection::execute (std::shared_ptr<Statement>& statement)
   throw (adt::RuntimeException, DatabaseException)
{
   return m_connection->execute(statement);
}

void dbms::GuardConnection::commit()
   throw (adt::RuntimeException, dbms::DatabaseException)
{
   m_connection->commit();
}
