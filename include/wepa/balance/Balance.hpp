// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2013 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/wepa
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#ifndef __wepa_balance_Balance_hpp
#define __wepa_balance_Balance_hpp

#include <vector>
#include <mutex>
#include <memory>

#include <wepa/adt/RuntimeException.hpp>
#include <wepa/adt/NamedObject.hpp>

namespace wepa {

namespace xml {
class Node;
}

namespace balance {

class Resource;

class Balance : public adt::NamedObject  {
   typedef std::vector <std::shared_ptr<Resource> > resource_container;

public:
   typedef resource_container::iterator resource_iterator;
   typedef resource_container::const_iterator const_resource_iterator;
   typedef std::lock_guard<std::mutex> lock_guard;

   virtual ~Balance () { m_resources.clear (); }

   bool add (const std::shared_ptr<Resource>& resource) throw (adt::RuntimeException);

   /**
    * It will call to pure virtual method \em do_initialize and do_initializer for every one of the associated resources.
    */
   virtual void initialize () throw (adt::RuntimeException);

   resource_iterator resource_begin(lock_guard&) noexcept { return m_resources.begin (); }
   resource_iterator resource_end(lock_guard&) noexcept { return m_resources.end (); }
   resource_iterator next (lock_guard&, resource_iterator ii) noexcept;
   static std::shared_ptr<Resource>& resource(resource_iterator ii) noexcept { return *ii; }

   size_t size (lock_guard&) const noexcept { return m_resources.size (); }
   size_t countAvailableResources (lock_guard&) const noexcept;

   const_resource_iterator resource_begin (lock_guard&) const noexcept { return m_resources.begin (); }
   const_resource_iterator resource_end (lock_guard&) const noexcept { return m_resources.end (); }
   static const std::shared_ptr<Resource>& resource(const_resource_iterator ii) noexcept { return *ii; }

   lock_guard getLockGuard() const noexcept { return lock_guard(m_mutex); }

   operator adt::StreamString () const noexcept { return asString (); }

   virtual adt::StreamString asString () const noexcept;
   virtual xml::Node& asXML (xml::Node& parent) const noexcept;

protected:
   Balance (const char* name) : adt::NamedObject (name) {;}

private:
   mutable std::recursive_mutex m_mutex;
   resource_container m_resources;

   Balance (const Balance&);
};

} /* namespace balance */
} /* namespace wepa */
#endif
