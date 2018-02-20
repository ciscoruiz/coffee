#ifndef __coffee_persistence_Eraser_hpp
#define __coffee_persistence_Eraser_hpp

#include <coffee/persistence/Accessor.hpp>

namespace coffee {

namespace dbms {
   class GuardStatement;
}

namespace persistence {

class Eraser : public Accessor {
public:
   virtual ~Eraser () {;}

   virtual dbms::ResultCode apply(dbms::GuardStatement& statement)
      throw(adt::RuntimeException, dbms::DatabaseException) = 0;

protected:
   Eraser (const char* name, TheStatement& statement, const ThePrimaryKey& primaryKey) :
      Accessor(name, statement, primaryKey) {;}
};

} /* namespace persistence */
} /* namespace coffee */
#endif
