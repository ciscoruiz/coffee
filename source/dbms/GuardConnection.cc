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
#include <wepa/logger/Logger.hpp>

#include <wepa/dbms/Connection.hpp>
#include <wepa/dbms/GuardConnection.hpp>

using namespace wepa;

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
