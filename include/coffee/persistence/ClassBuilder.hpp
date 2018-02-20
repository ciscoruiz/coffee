#ifndef __coffee_persistence_ClassBuilder_hpp
#define __coffee_persistence_ClassBuilder_hpp

#include <memory>

#include <coffee/adt/RuntimeException.hpp>

#include <coffee/dbms/datatype/Set.hpp>

#include <coffee/persistence/Class.hpp>

namespace coffee {

namespace persistence {

class ClassBuilder : public dbms::datatype::Set {
public:
   explicit ClassBuilder(const std::string& className) : m_className (className) {;}
   
   ClassBuilder& set(std::shared_ptr<PrimaryKey>& primaryKey) throw (adt::RuntimeException);
   
   ClassBuilder& add(std::shared_ptr<dbms::datatype::Abstract> data) throw (adt::RuntimeException) {
      insert(data);
      return *this;
   }

   const std::string& getClassName() const noexcept { return m_className; }
   const std::shared_ptr<PrimaryKey>& getPrimaryKey() const noexcept { return m_primaryKey; }

   std::shared_ptr<Class> build() const throw (adt::RuntimeException);
   
private:
   const std::string m_className;
   std::shared_ptr<PrimaryKey> m_primaryKey;
};

}
}
#endif
