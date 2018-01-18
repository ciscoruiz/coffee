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
#ifndef __wepa_mock_MockConnection_hpp
#define __wepa_mock_MockConnection_hpp

#include <vector>

#include <wepa/adt/StreamString.hpp>

#include <wepa/dbms/Connection.hpp>

#include "MockLowLevelRecord.hpp"

namespace wepa {

namespace dbms {
   class Database;
}
namespace mock {

class MockDatabase;

class MockConnection : public dbms::Connection {
public:
   enum OpCode {  Write, Delete };

   MockConnection(MockDatabase& database, const std::string& name, const char* user, const char* password);

   int operation_size() const noexcept { return m_operations.size(); }

   void manualBreak() noexcept { m_isOpen = false; }
   bool isAvailable() const noexcept { return m_isOpen; }
   void avoidRecovery() noexcept { m_avoidRecovery = true; }
   void clearAvoidRecovery() noexcept { m_avoidRecovery = false; }

   unsigned int getCommitCounter() const noexcept { return m_commitCounter; }
   unsigned int getRollbackCounter() const noexcept { return m_rollbackCounter; }
   unsigned int getOpenCounter() const noexcept { return m_openCounter; }
   unsigned int getCloseCounter() const noexcept { return m_closeCounter; }

   adt::StreamString asString() const noexcept {
      adt::StreamString result("MockConnection {");
      result << dbms::Connection::asString();
      result << " | CommitCounter=" << m_commitCounter;
      result << " | RollbackCounter=" << m_rollbackCounter;
      return result << " }";
   }

   mock::MockLowLevelContainer& getContainer() throw(adt::RuntimeException);

   void addOperation(const OpCode opCode, const MockLowLevelRecord& record) noexcept {
      m_operations.push_back(Operation(opCode, record));
   }

private:
   typedef std::pair <OpCode, mock::MockLowLevelRecord> Operation;
   typedef std::vector <Operation> Operations;

   unsigned int m_commitCounter;
   unsigned int m_rollbackCounter;
   unsigned int m_openCounter;
   unsigned int m_closeCounter;

   bool m_isOpen;
   mock::MockLowLevelContainer& m_container;
   Operations m_operations;
   bool m_avoidRecovery;

   void open() throw(dbms::DatabaseException);
   void close() noexcept;
   void do_commit() throw (adt::RuntimeException, dbms::DatabaseException);
   void do_rollback() noexcept;

   friend class MyReadStatement;
   friend class MyWriteStatement;
};

} /* namespace mock */
} /* namespace wepa */
#endif
