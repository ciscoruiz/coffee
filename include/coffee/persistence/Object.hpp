#ifndef __coffee_persistence_Object_hpp
#define __coffee_persistence_Object_hpp

#include <functional>

#include <coffee/adt/StreamString.hpp>
#include <coffee/adt/RuntimeException.hpp>

#include <coffee/dbms/datatype/Set.hpp>

namespace coffee {
   
namespace persistence {

class PrimaryKey;
class Class;


//// See http://stackoverflow.com/questions/1319876/weird-c-templating-issues
//template <typename _T> class AutoObject;

class Object : public dbms::datatype::Set {
public:
   Object(const Class& _class, const std::shared_ptr<PrimaryKey>& primaryKey, const dbms::datatype::Set& members);

   ~Object() {
       m_primaryKey.reset();
   }

   const Class& getClass() const noexcept { return std::ref(m_class); }
   const std::shared_ptr<PrimaryKey>& getPrimaryKey() const throw(adt::RuntimeException) { return m_primaryKey; }

   std::string getInternalId() const noexcept;
   
   operator adt::StreamString() const noexcept { return asString(); }

   adt::StreamString asString() const noexcept;

private:
   const Class& m_class;
   std::shared_ptr<PrimaryKey> m_primaryKey;
};

} /* namespace persistence */
} /* namespace coffee */
#endif
