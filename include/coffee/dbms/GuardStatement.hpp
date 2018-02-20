#ifndef __coffee_dbms_GuardStatement_hpp
#define __coffee_dbms_GuardStatement_hpp

#include <memory>

#include <coffee/adt/RuntimeException.hpp>
#include <coffee/dbms/ResultCode.hpp>

namespace coffee {
namespace dbms {

namespace datatype {
class Abstract;
}

class GuardConnection;

class Statement;

/**
 * This class will be previously protected by the GuardConnection it will assure that it will no be possible
 * a deadlock.
 */
class GuardStatement {
public:
   GuardStatement(GuardConnection& guardConnection, std::shared_ptr<Statement>&) throw(adt::RuntimeException);
   ~GuardStatement();

   GuardConnection& getGuardConnection() noexcept { return m_guardConnection; }
   std::shared_ptr<Statement> operator->() noexcept { return m_statement; }

   std::shared_ptr<datatype::Abstract>& getInputData(const int pos) throw(adt::RuntimeException);
   const std::shared_ptr<datatype::Abstract>& getOutputData(const int pos) const throw(adt::RuntimeException);

   ResultCode execute() throw(adt::RuntimeException, DatabaseException);

   bool fetch() throw(adt::RuntimeException, DatabaseException);

   bool setRequiresCommit(const bool requiresCommit) noexcept;

private:
   GuardConnection& m_guardConnection;
   std::shared_ptr<Statement> m_statement;
};
}
}

#endif
