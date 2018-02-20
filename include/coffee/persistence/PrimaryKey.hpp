#ifndef __coffee_persistence_PrimaryKey_hpp
#define __coffee_persistence_PrimaryKey_hpp

#include <vector>
#include <memory>

#include <coffee/adt/RuntimeException.hpp>
#include <coffee/adt/StreamString.hpp>

#include <coffee/dbms/datatype/Set.hpp>

namespace coffee {

namespace dbms {
   namespace datatype {
      class Abstract;
   }
}

namespace persistence {

class PrimaryKeyBuilder;

class PrimaryKey : public dbms::datatype::Set {
public:
   struct EqualSharedPointer {
      bool operator()(const std::shared_ptr<PrimaryKey>& lhs, const std::shared_ptr<PrimaryKey>& rhs) const {
         return lhs->compare(rhs) == 0;
      }
   };
   struct HashSharedPointer {
      size_t operator()(const std::shared_ptr<PrimaryKey>& primaryKey) const { return primaryKey->hash(); }
   };

   explicit PrimaryKey(const PrimaryKeyBuilder& builder);
   PrimaryKey (const PrimaryKey& other) : dbms::datatype::Set(other) {;}

   PrimaryKey& operator= (const PrimaryKey& other) throw (adt::Exception) { Set::operator=(other); return *this; }
   bool operator== (const PrimaryKey& other) const throw (adt::RuntimeException) { return compare(other) == 0; }
   bool operator< (const PrimaryKey& other) const throw (adt::RuntimeException){ return compare(other) < 0; }
   bool matches(const PrimaryKey& other) const noexcept;

   size_t hash() const noexcept;

   operator adt::StreamString () const noexcept { return asString (); }
   adt::StreamString asString () const noexcept;
};

} /* namespace persistence */
} /* namespace coffee */

#endif
