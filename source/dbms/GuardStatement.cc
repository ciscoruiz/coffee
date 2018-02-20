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


