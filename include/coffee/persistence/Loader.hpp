#ifndef __coffee_persistence_Loader_hpp
#define __coffee_persistence_Loader_hpp

#include <coffee/persistence/Accessor.hpp>

#include <coffee/adt/RuntimeException.hpp>

namespace coffee {

namespace dbms {
   class GuardStatement;
}

namespace persistence {



class Loader : public Accessor {
public:
   virtual ~Loader () {;}

   virtual dbms::ResultCode apply(dbms::GuardStatement& statement, TheObject& object)
      throw(adt::RuntimeException, dbms::DatabaseException) = 0;

   virtual bool hasToRefresh (dbms::GuardStatement& statement, TheObject& object)
      throw (adt::RuntimeException, dbms::DatabaseException) = 0;

   const TheClass& getClass() const noexcept { return m_class; }
   
protected:
   Loader (const char* name, TheStatement& statement, const ThePrimaryKey& primaryKey, const TheClass& clazz) :
      Accessor(name, statement, primaryKey),
      m_class(clazz)
   {;}
   
private:
   TheClass m_class;
};

} /* namespace persistence */
} /* namespace coffee */
#endif
