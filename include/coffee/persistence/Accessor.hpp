#ifndef __coffee_persistence_Accessor_hpp
#define __coffee_persistence_Accessor_hpp

#include <vector>

#include <coffee/adt/RuntimeException.hpp>
#include <coffee/adt/NamedObject.hpp>

#include <coffee/dbms/DatabaseException.hpp>
#include <coffee/dbms/ResultCode.hpp>

#include <coffee/persistence/PrimaryKey.hpp>

namespace coffee {

namespace adt {
   class DataBlock;
   class Second;
}

namespace dbms {
   class Statement;
   class Connection;
   class GuardConnection;
   class GuardStatement;

   namespace datatype {
      class Abstract;
   }
}

namespace persistence {

class Class;
class Object;

class Accessor : public adt::NamedObject {
public:
   typedef std::shared_ptr<dbms::Connection> TheConnection;
   typedef std::shared_ptr<dbms::Statement> TheStatement;
   typedef std::shared_ptr<Class> TheClass;
   typedef std::shared_ptr<Object> TheObject;
   typedef std::shared_ptr<PrimaryKey> ThePrimaryKey;
   
   TheStatement& getStatement() noexcept { return m_statement; }
   const ThePrimaryKey& getPrimaryKey() const throw(adt::RuntimeException) { return m_primaryKey; }
   
   bool hasAutoCommit() const noexcept { return m_autoCommit; }
   bool setAutoCommit(const bool value) { bool result(m_autoCommit); m_autoCommit = value; return result; }
   bool enableAutoCommit() noexcept { return setAutoCommit(true); }
   void disableAutoCommit() noexcept { setAutoCommit(false); }

   Accessor(const Accessor&) = delete;

protected:
   Accessor(const char* name, TheStatement& statement, const ThePrimaryKey& primaryKey) :
      adt::NamedObject(name),
      m_statement(statement),
      m_primaryKey(primaryKey),
      m_autoCommit(false)
   {;}

   Accessor(const char* name, const ThePrimaryKey& primaryKey) :
      adt::NamedObject(name),
      m_statement(nullptr),
      m_primaryKey(primaryKey),
      m_autoCommit(false)
   {;}
      
private:
   TheStatement m_statement;
   const ThePrimaryKey m_primaryKey;
   bool m_autoCommit;
};

} /* namespace persistence */
} /* namespace coffee */

#endif


