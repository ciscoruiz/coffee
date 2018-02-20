#include <coffee/persistence/Repository.hpp>
#include <coffee/persistence/Storage.hpp>

#include <coffee/logger/Logger.hpp>

#include <coffee/xml/Node.hpp>
#include <coffee/xml/Attribute.hpp>

using namespace coffee;

std::shared_ptr<persistence::Storage> persistence::Repository::createStorage(const std::string& ident, const int maxCacheSize)
   throw(adt::RuntimeException)
{
   storage_iterator ii = m_storages.find(ident);

   if(ii != m_storages.end()) {
      COFFEE_THROW_EXCEPTION("Ident=" << ident << " has already been used");
   }

   std::shared_ptr<Storage> result = std::make_shared<Storage>(ident, maxCacheSize);
   LOG_DEBUG(result->asString());
   m_storages[ident] = result;

   return result;
}

std::shared_ptr<persistence::Storage>& persistence::Repository::findStorage(const std::string& ident)
   throw(adt::RuntimeException)
{

   storage_iterator ii = m_storages.find(ident);

   if(ii == m_storages.end()) {
      COFFEE_THROW_EXCEPTION("ID=" << ident << " has not been defined yet");
   }

   return std::ref(Repository::storage(ii));
}

adt::StreamString persistence::Repository::asString() const
   noexcept
{
   adt::StreamString result("persistence.Repository { ");
   result << adt::NamedObject::asString();
   result << " | N-Size=" << m_storages.size();
   return result += " }";
}

std::shared_ptr<xml::Node> persistence::Repository::asXML(std::shared_ptr<xml::Node>& parent) const
   noexcept
{
   std::shared_ptr<xml::Node> result = parent->createChild("persistence.Repository");

   result->createAttribute("Name", getName());

   for(const_storage_iterator ii = storage_begin(), maxii = storage_end(); ii != maxii; ++ ii)  {
      storage(ii)->asXML(result);
   }

   return result;
}
