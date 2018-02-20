#ifndef __coffee_persistence_Storage_hpp
#define __coffee_persistence_Storage_hpp

#include <memory>

#include <coffee/adt/NamedObject.hpp>

#include <coffee/adt/pattern/lru/Cache.hpp>

#include <coffee/persistence/PrimaryKey.hpp>
#include <coffee/persistence/Accessor.hpp>

#include <coffee/dbms/DatabaseException.hpp>

namespace coffee {
   namespace xml {
      class Node;
   }
   namespace dbms {
      class Connection;
      class GuardConnection;
   }

namespace persistence {

class Repository;
class Object;
class Loader;
class Recorder;
class Eraser;
class Creator;

class Storage : public adt::NamedObject {
public:

private:
   typedef adt::pattern::lru::Cache<
         Accessor::ThePrimaryKey,
         Accessor::TheObject,
         persistence::PrimaryKey::HashSharedPointer,
         persistence::PrimaryKey::EqualSharedPointer> Cache;

   static const int UpperLimitForMaxCacheSize = 4 * 1024;
   static const int LowerLimitForMaxCacheSize = 16;

public:

   static const int DefaultMaxCacheSize = 128;

   Storage(const std::string& name, const int maxCacheSize);
   ~Storage();

   unsigned int getHitCounter() const noexcept { return m_hitCounter; }
   unsigned int getFaultCounter() const noexcept { return m_faultCounter; }

   Accessor::TheObject load(Accessor::TheConnection& connection, Loader& loader)
      throw(adt::RuntimeException, dbms::DatabaseException);
   void save(Accessor::TheConnection& connection, Recorder& recorder)
      throw(adt::RuntimeException, dbms::DatabaseException);
   void erase(Accessor::TheConnection& connection, Eraser& eraser)
      throw(adt::RuntimeException, dbms::DatabaseException);

   void save(dbms::GuardConnection& connection, Recorder& recorder)
      throw(adt::RuntimeException, dbms::DatabaseException);
   void erase(dbms::GuardConnection& connection, Eraser& eraser)
      throw(adt::RuntimeException, dbms::DatabaseException);

   operator adt::StreamString() const noexcept { return asString(); }

   adt::StreamString asString() const noexcept;

   std::shared_ptr<xml::Node> asXML(std::shared_ptr<xml::Node>& parent) const noexcept;

   Storage() = delete;
   Storage(const Storage&) = delete;
   
private:
   Cache m_cache;
   mutable unsigned int m_hitCounter;
   mutable unsigned int m_faultCounter;
};

} /* namespace persistence */
} /* namespace coffee */
#endif
