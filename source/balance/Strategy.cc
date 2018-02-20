
#include <coffee/xml/Node.hpp>

#include <coffee/balance/Strategy.hpp>
#include <coffee/balance/ResourceList.hpp>

using namespace coffee;

//virtual
adt::StreamString balance::Strategy::asString() const noexcept
{
   adt::StreamString result("balance.Strategy { ");
   result += adt::NamedObject::asString();
   result += "|";
   result += m_resources->asString();
   return result += " }";

}

//virtual
std::shared_ptr<xml::Node> balance::Strategy::asXML(std::shared_ptr<xml::Node>& parent) const noexcept
{
   std::shared_ptr<xml::Node> result = parent->createChild(this->getName());
   m_resources->asXML(result);
   return result;
}
