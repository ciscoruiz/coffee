#ifndef __coffee_persistence_PrimaryKeyBuilder_hpp
#define __coffee_persistence_PrimaryKeyBuilder_hpp

#include <coffee/adt/RuntimeException.hpp>

#include <coffee/dbms/datatype/Set.hpp>

#include <coffee/persistence/PrimaryKey.hpp>

namespace coffee {
    
namespace persistence {

/**
 * @todo write docs
 */
class PrimaryKeyBuilder : public dbms::datatype::Set {
public:
   /**
    * Default constructor
    */
   PrimaryKeyBuilder() {;}

   PrimaryKeyBuilder& add(std::shared_ptr<dbms::datatype::Abstract> data) throw (adt::RuntimeException) {
      insert(data);
      return *this;
   }
    
   std::shared_ptr<persistence::PrimaryKey> build() const throw (adt::RuntimeException);
    
private:
    friend class PrimaryKey;
};

}
}

#endif // PRIMARYKEYBUILDER_H
