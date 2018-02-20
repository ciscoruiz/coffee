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
