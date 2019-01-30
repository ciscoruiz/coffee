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

#ifndef __coffee_balance_ResourceContainer_hpp
#define __coffee_balance_ResourceContainer_hpp

#include <vector>
#include <mutex>
#include <memory>

#include <coffee/basis/RuntimeException.hpp>
#include <coffee/basis/NamedObject.hpp>

namespace coffee {

namespace xml {
class Node;
}

namespace balance {

class Resource;
class GuardResourceContainer;

/**
 * List of resources with exclusive access.
 */
class ResourceContainer : public basis::NamedObject  {
   typedef std::vector <std::shared_ptr<Resource> > resource_container;

public:
   typedef resource_container::iterator resource_iterator;
   typedef resource_container::const_iterator const_resource_iterator;

   /**
    * Constructor
    * \param name Logical name.
    */
   explicit ResourceContainer(const char* name) : basis::NamedObject(name) {;}

   /**
    * Destructor.
    */
   virtual ~ResourceContainer() { m_resources.clear(); }

   /**
    * Add resource to the list. It is thread-safe.
    */
   bool add(std::shared_ptr<Resource> resource) throw(basis::RuntimeException);

   /**
    * It will call to pure virtual method \em do_initialize and do_initializer for every one of the associated resources.
    */
   virtual void initialize() throw(basis::RuntimeException);

   /**
    * \return resource_iterator to the first attached resource.
    */
   resource_iterator resource_begin(GuardResourceContainer&) noexcept { return m_resources.begin(); }

   /**
    * \return resource_iterator to the last attached resource.
    */
   resource_iterator resource_end(GuardResourceContainer&) noexcept { return m_resources.end(); }

   /**
    * Advance one position in the iterator, if this position reaches the value #resource_end then
    * it will return to the first value #resource_begin.
    */
   resource_iterator next(GuardResourceContainer&, resource_iterator ii) noexcept;

   /**
    * \return the resource addressed by the resource_iterator.
    * \warning the value ii must be contained in [#resource_begin, #resource_end)
    */
   static std::shared_ptr<Resource>& resource(resource_iterator ii) noexcept { return *ii; }

   /**
    * \return the Resource at the position received as parameter.
    */
   std::shared_ptr<Resource>& at(GuardResourceContainer&, const resource_container::size_type index) { return m_resources.at(index); }

   /**
    * \return The number of resources registered in this list.
    */
   size_t size(GuardResourceContainer&) const noexcept { return m_resources.size(); }

   /**
    * \return The number of available resources.
    */
   size_t countAvailableResources(GuardResourceContainer&) const noexcept;

   /**
    * \return resource_iterator to the first attached resource.
    */
   const_resource_iterator resource_begin(GuardResourceContainer&) const noexcept { return m_resources.begin(); }

   /**
    * \return resource_iterator to the last attached resource.
    */
   const_resource_iterator resource_end(GuardResourceContainer&) const noexcept { return m_resources.end(); }

   /**
    * \return the resource addressed by the resource_iterator.
    * \warning the value ii must be contained in [#resource_begin, #resource_end)
    */
   static const std::shared_ptr<Resource>& resource(const_resource_iterator ii) noexcept { return *ii; }

   /**
    * \return the Resource at the position received as parameter.
    */
   const std::shared_ptr<Resource>& at(GuardResourceContainer&, const resource_container::size_type index) const { return m_resources.at(index); }

   /**
    * \return Summarize information of this instance in a StreamString.
    */
   operator basis::StreamString() const noexcept { return asString(); }

   /**
    * \return Summarize information of this instance in a StreamString.
    */
   virtual basis::StreamString asString() const noexcept;

   /**
    * \return Summarize information of this instance in a coffee::xml::Node.
    */
   virtual std::shared_ptr<xml::Node> asXML(std::shared_ptr<xml::Node>& parent) const throw(basis::RuntimeException);

private:
   mutable std::mutex m_mutex;
   resource_container m_resources;

   ResourceContainer(const ResourceContainer&);

   friend class GuardResourceContainer;
};

} /* namespace balance */
} /* namespace coffee */
#endif
