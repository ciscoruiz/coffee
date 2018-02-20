#ifndef __coffee_mock_MockConnection_hpp
#define __coffee_mock_MockConnection_hpp

#include <vector>

#include <coffee/adt/StreamString.hpp>

#include <coffee/dbms/Connection.hpp>

#include "MockLowLevelRecord.hpp"

namespace coffee {

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
} /* namespace coffee */
#endif
