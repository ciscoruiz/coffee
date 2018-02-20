#ifndef __coffee_dbms_GuardConnection_hpp
#define __coffee_dbms_GuardConnection_hpp

#include <memory>
#include <functional>

#include <coffee/adt/RuntimeException.hpp>
#include <coffee/dbms/ResultCode.hpp>

namespace coffee {
namespace dbms {

class Connection;
class GuardStatement;

class GuardConnection {
public:
   explicit GuardConnection (std::shared_ptr<Connection> connection) throw (adt::RuntimeException);
   ~GuardConnection ();

   std::shared_ptr<Connection>& operator-> () noexcept { return std::ref(m_connection); }

   int setMaxCommitPending (const int maxCommitPending) noexcept;
   void clearMaxCommitPending () noexcept;
   void commit() throw (adt::RuntimeException, DatabaseException);

   int getCountLinkedStatement () const noexcept { return m_countLinkedStatement; }

private:
   std::shared_ptr<Connection> m_connection;
   int m_countLinkedStatement;

   void linkStatement () noexcept { m_countLinkedStatement ++; }
   void unlinkStatement () noexcept { m_countLinkedStatement --; }

   ResultCode execute (std::shared_ptr<Statement>& statement) throw (adt::RuntimeException, DatabaseException);

   friend class GuardStatement;
};
}
}

#endif
