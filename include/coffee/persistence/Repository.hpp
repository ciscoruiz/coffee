#ifndef __coffee_persistence_Repository_hpp
#define __coffee_persistence_Repository_hpp

#include <unordered_map>
#include <memory>
#include <functional>

#include <coffee/adt/NamedObject.hpp>
#include <coffee/adt/RuntimeException.hpp>

namespace coffee {

namespace xml {
class Node;
}

namespace persistence {

class Storage;

class Repository : public adt::NamedObject {
public:
   explicit Repository(const char* name) : NamedObject(name) {;}
   explicit Repository(const std::string& name) : NamedObject(name) {;}

   std::shared_ptr<Storage> createStorage(const std::string& name, const int maxCacheSize) throw(adt::RuntimeException);
   std::shared_ptr<Storage>& findStorage(const std::string& name) throw(adt::RuntimeException);

   adt::StreamString asString() const noexcept;

   std::shared_ptr<xml::Node> asXML(std::shared_ptr<xml::Node>& parent) const noexcept;

   Repository(const Repository&) = delete;

private:
   typedef std::unordered_map<std::string, std::shared_ptr<Storage> > Storages;
   typedef Storages::iterator storage_iterator;
   typedef Storages::const_iterator const_storage_iterator;

   Storages m_storages;

   storage_iterator storage_begin() noexcept { return m_storages.begin(); }
   storage_iterator storage_end() noexcept { return m_storages.end(); }
   static std::shared_ptr<Storage>& storage(storage_iterator ii) noexcept { return std::ref(ii->second); }

   const_storage_iterator storage_begin() const noexcept { return m_storages.begin(); }
   const_storage_iterator storage_end() const noexcept { return m_storages.end(); }
   static const std::shared_ptr<Storage>& storage(const_storage_iterator ii) noexcept { return std::ref(ii->second); }
};

} /* namespace persistence */
} /* namespace coffee */
#endif
