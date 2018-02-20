#ifndef __coffee_balance_Strategy_hpp
#define __coffee_balance_Strategy_hpp

#include <memory>

#include <coffee/adt/NamedObject.hpp>
#include <coffee/balance/ResourceUnavailableException.hpp>

namespace coffee {

namespace xml {
   class Node;
}

namespace balance {

class Resource;
class GuardResourceList;
class ResourceList;

class Strategy : public adt::NamedObject {
public:
   virtual std::shared_ptr<Resource> apply(GuardResourceList& guard) throw (ResourceUnavailableException) = 0;

   virtual adt::StreamString asString () const noexcept;
   virtual std::shared_ptr<xml::Node> asXML (std::shared_ptr<xml::Node>& parent) const noexcept;

   std::shared_ptr<ResourceList>& getResourceList() { return m_resources; }

protected:
   std::shared_ptr<ResourceList> m_resources;

   Strategy(const std::string& name, std::shared_ptr<ResourceList>& resources) : adt::NamedObject(name), m_resources(resources) {;}
   explicit Strategy(const std::string& name) : adt::NamedObject(name) {;}
   void setResourceList(std::shared_ptr<ResourceList>& resources) { m_resources = resources; }
};

}
}

#endif /* __coffee_balance_Strategy_hpp */
