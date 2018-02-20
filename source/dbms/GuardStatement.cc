
#include <coffee/dbms/Statement.hpp>
#include <coffee/dbms/GuardStatement.hpp>
#include <coffee/dbms/GuardConnection.hpp>
#include <coffee/dbms/Connection.hpp>

using namespace coffee;

dbms::GuardStatement::GuardStatement(GuardConnection& guardConnection, std::shared_ptr<Statement>& statement)  throw(adt::RuntimeException) :
   m_guardConnection(guardConnection),
   m_statement(statement)
{
   if(guardConnection->isAvailable() == false) {
      COFFEE_THROW_EXCEPTION(guardConnection->asString() << " | Connection is not available");
   }

   guardConnection.linkStatement();

   statement->lock();
}

dbms::GuardStatement::~GuardStatement()
{
   m_statement->unlock();
   m_guardConnection.unlinkStatement();
}

std::shared_ptr<dbms::datatype::Abstract>& dbms::GuardStatement::getInputData(const int pos)
   throw(adt::RuntimeException)
{
   return m_statement->getInputData(*this, pos);
}

const std::shared_ptr<dbms::datatype::Abstract>& dbms::GuardStatement::getOutputData(const int pos) const
   throw(adt::RuntimeException)
{
   return m_statement->getOutputData(*this, pos);
}

dbms::ResultCode dbms::GuardStatement::execute()
   throw(adt::RuntimeException, DatabaseException)
{
   return m_guardConnection.execute(m_statement);
}

bool dbms::GuardStatement::fetch()
   throw(adt::RuntimeException, DatabaseException)
{
   return m_statement->fetch();
}

bool dbms::GuardStatement::setRequiresCommit(const bool requiresCommit) noexcept
{
   return m_statement->setRequiresCommit(requiresCommit);
}


