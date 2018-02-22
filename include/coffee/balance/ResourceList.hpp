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

#ifndef __coffee_balance_ResourceList_hpp
#define __coffee_balance_ResourceList_hpp

#include <vector>
#include <mutex>
#include <memory>

#include <coffee/adt/RuntimeException.hpp>
#include <coffee/adt/NamedObject.hpp>

namespace coffee {

namespace xml {
class Node;
}

namespace balance {

class Resource;
class GuardResourceList;

class ResourceList : public adt::NamedObject  {
   typedef std::vector <std::shared_ptr<Resource> > resource_container;

public:
   typedef resource_container::iterator resource_iterator;
   typedef resource_container::const_iterator const_resource_iterator;

   explicit ResourceList(const char* name) : adt::NamedObject(name) {;}
   virtual ~ResourceList() { m_resources.clear(); }

   bool add(std::shared_ptr<Resource> resource) throw(adt::RuntimeException);

   /**
    * It will call to pure virtual method \em do_initialize and do_initializer for every one of the associated resources.
    */
   virtual void initialize() throw(adt::RuntimeException);

   resource_iterator resource_begin(GuardResourceList&) noexcept { return m_resources.begin(); }
   resource_iterator resource_end(GuardResourceList&) noexcept { return m_resources.end(); }
   resource_iterator next(GuardResourceList&, resource_iterator ii) noexcept;
   static std::shared_ptr<Resource>& resource(resource_iterator ii) noexcept { return *ii; }

   size_t size(GuardResourceList&) const noexcept { return m_resources.size(); }
   size_t countAvailableResources(GuardResourceList&) const noexcept;
   std::shared_ptr<Resource>& at(GuardResourceList&, const resource_container::size_type index) { return m_resources.at(index); }

   const_resource_iterator resource_begin(GuardResourceList&) const noexcept { return m_resources.begin(); }
   const_resource_iterator resource_end(GuardResourceList&) const noexcept { return m_resources.end(); }
   static const std::shared_ptr<Resource>& resource(const_resource_iterator ii) noexcept { return *ii; }
   const std::shared_ptr<Resource>& at(GuardResourceList&, const resource_container::size_type index) const { return m_resources.at(index); }

   operator adt::StreamString() const noexcept { return asString(); }

   virtual adt::StreamString asString() const noexcept;
   virtual std::shared_ptr<xml::Node> asXML(std::shared_ptr<xml::Node>& parent) const noexcept;

private:
   mutable std::mutex m_mutex;
   resource_container m_resources;

   ResourceList(const ResourceList&);

   friend class GuardResourceList;
};

} /* namespace balance */
} /* namespace coffee */
#endif
