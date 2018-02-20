#include <coffee/balance/Resource.hpp>

#include <coffee/adt/StreamString.hpp>
#include <coffee/adt/AsString.hpp>

#include <coffee/xml/Node.hpp>
#include <coffee/xml/Attribute.hpp>

using namespace coffee;

//virtual
adt::StreamString balance::Resource::asString() const
   noexcept
{
   adt::StreamString result("balance::Resource {");
   result += adt::NamedObject::asString();
   result += " } | Available = ";
   result += adt::AsString::apply(isAvailable());
   return result.append("}");
}

//virtual
std::shared_ptr<xml::Node> balance::Resource::asXML(std::shared_ptr<xml::Node>& parent) const
   noexcept
{
   std::shared_ptr<xml::Node> result = parent->createChild("balance.Resource");
   result->createAttribute("Name", this->getName());
   result->createAttribute("IsAvailable", isAvailable());
   return result;
}
