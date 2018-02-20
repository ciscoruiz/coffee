#ifndef __coffee_balance_Resource_hpp
#define __coffee_balance_Resource_hpp

#include <memory>

#include <coffee/adt/NamedObject.hpp>
#include <coffee/adt/RuntimeException.hpp>

namespace coffee {

namespace xml {
class Node;
}

namespace balance {

class Resource : public adt::NamedObject {
public:
   virtual ~Resource () {;}

   virtual void initialize () throw (adt::RuntimeException) {;}

   virtual bool isAvailable () const noexcept = 0;

   operator adt::StreamString () const noexcept { return asString (); }

   virtual adt::StreamString asString () const noexcept;
   virtual std::shared_ptr<xml::Node> asXML (std::shared_ptr<xml::Node>& parent) const noexcept;

protected:
   explicit Resource(const std::string& name) : adt::NamedObject(name) {;}

};

} /* namespace balance */
} /* namespace coffee */
#endif

