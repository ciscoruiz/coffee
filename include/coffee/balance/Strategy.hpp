// MIT License
// 
// Copyright (c) 2018 Francisco Ruiz (francisco.ruiz.rayo@gmail.com)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#ifndef __coffee_balance_Strategy_hpp
#define __coffee_balance_Strategy_hpp

#include <memory>

#include <coffee/basis/NamedObject.hpp>
#include <coffee/balance/ResourceUnavailableException.hpp>

namespace coffee {

namespace xml {
   class Node;
}

namespace balance {

class Resource;
class GuardResourceList;
class ResourceList;

/**
 * Generic strategy for selecting a resource from a ResourceList.
 */
class Strategy : public basis::NamedObject {
public:
   /**
    * Method that will choose one Resource from the ResourceList.
    * It will be done in exclusive access protected by the guard.
    */
   virtual std::shared_ptr<Resource> apply(GuardResourceList& guard) throw (ResourceUnavailableException) = 0;

   /**
    * \return Summarize information of this instance in a StreamString.
    */
   virtual basis::StreamString asString () const noexcept;

   /**
    * \return Summarize information of this instance in a coffee::xml::Node.
    */
   virtual std::shared_ptr<xml::Node> asXML (std::shared_ptr<xml::Node>& parent) const noexcept;

   /**
    * \return The resource list associated to this instance.
    */
   std::shared_ptr<ResourceList>& getResourceList() { return m_resources; }

protected:
   std::shared_ptr<ResourceList> m_resources;

   /**
    * Constuctor.
    * \param name Logical name.
    * \param resources List of resources to work with.
    */
   Strategy(const std::string& name, std::shared_ptr<ResourceList>& resources) : basis::NamedObject(name), m_resources(resources) {;}

   /**
    * Constuctor.
    * \param name Logical name.
    */
   explicit Strategy(const std::string& name) : basis::NamedObject(name) {;}

   /**
    * Set the resource list to work with.
    */
   void setResourceList(std::shared_ptr<ResourceList>& resources) { m_resources = resources; }
};

}
}

#endif /* __coffee_balance_Strategy_hpp */
