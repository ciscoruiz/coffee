#ifndef __coffee_persistence_Recorder_hpp
#define __coffee_persistence_Recorder_hpp

#include <coffee/persistence/Accessor.hpp>
#include <coffee/persistence/Object.hpp>

#include <coffee/adt/RuntimeException.hpp>

namespace coffee {

namespace dbms {
   class GuardStatement;
}
namespace persistence {

class Object;

class Recorder : public Accessor {
public:
   virtual ~Recorder () {;}

   const Accessor::TheObject& getObject() const noexcept { return m_object; }

   virtual dbms::ResultCode apply(dbms::GuardStatement& statement)
      throw(adt::RuntimeException, dbms::DatabaseException) = 0;

protected:
   Recorder (const char* name, TheStatement& statement, const Accessor::TheObject& object) :
      Accessor(name, statement, object->getPrimaryKey()),
      m_object(object)
   {;}

private:
   Accessor::TheObject m_object;
};

} /* namespace persistence */
} /* namespace coffee */
#endif
