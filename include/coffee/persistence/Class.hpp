#ifndef __coffee_persistence_Class_hpp
#define __coffee_persistence_Class_hpp

#include <mutex>
#include <memory>
#include <vector>

#include <coffee/adt/NamedObject.hpp>
#include <coffee/adt/RuntimeException.hpp>

#include <coffee/dbms/datatype/Abstract.hpp>
#include <coffee/dbms/datatype/Set.hpp>

namespace coffee {

namespace xml {
   class Node;
}

namespace persistence {

class Object;
class Accessor;
class ClassBuilder;
class PrimaryKey;

class Class : public adt::NamedObject {
public:
   explicit Class(const ClassBuilder& classBuilder);

   virtual ~Class() {;}

   adt::StreamString asString() const noexcept;
      
   std::shared_ptr<PrimaryKey> createPrimaryKey() const throw ();
   std::shared_ptr<Object> createObject(const std::shared_ptr<PrimaryKey>& primaryKey) const throw(adt::RuntimeException);

   std::shared_ptr<xml::Node> asXML(std::shared_ptr<xml::Node>& parent) const noexcept;

   Class(const Class&) = delete;

private:
   std::shared_ptr<PrimaryKey> m_primaryKey;
   dbms::datatype::Set m_members;

};

} /* namespace persistence */
} /* namespace coffee */
#endif
