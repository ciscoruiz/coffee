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
#ifndef __wepa_balance_BalanceIf_hpp
#define __wepa_balance_BalanceIf_hpp

#include <vector>
#include <mutex>

#include <wepa/adt/RuntimeException.hpp>
#include <wepa/adt/NamedObject.hpp>

namespace wepa {

namespace xml {
class Node;
}

namespace balance {

class Resource;

class BalanceIf : public adt::NamedObject  {
   typedef std::vector <Resource*> resource_container;

   static const int NullKey;

public:
   typedef resource_container::iterator resource_iterator;
   typedef resource_container::const_iterator const_resource_iterator;
   struct Requires{ enum _v { None, Key = 1, PositiveKey = 2 }; };

   virtual ~BalanceIf () { m_resources.clear (); }

   bool add (Resource* resource) throw (adt::RuntimeException);

   virtual void populate () throw (adt::RuntimeException) {;}

   /**
    * It will call to pure virtual method \em do_initialize and do_initializer for every one of the associated resources.
    */
   void initialize () throw (adt::RuntimeException);

   Resource* apply (const int key = NullKey) throw (adt::RuntimeException);

   bool contains (const Resource* resource) const throw ();

   resource_iterator resource_begin () throw () { return m_resources.begin (); }
   resource_iterator resource_end () throw () { return m_resources.end (); }
   static Resource* resource (resource_iterator ii) throw () { return *ii; }

   size_t size () const throw () { return m_resources.size (); }
   size_t countAvailableResources () const throw ();

   const_resource_iterator resource_begin () const throw () { return m_resources.begin (); }
   const_resource_iterator resource_end () const throw () { return m_resources.end (); }
   static const Resource* resource (const_resource_iterator ii) throw () { return *ii; }

   virtual adt::StreamString asString () const throw ();
   virtual xml::Node& asXML (xml::Node& parent) const throw ();

protected:
   BalanceIf (const char* name, const Requires::_v requires) : adt::NamedObject (name), m_requires (requires) {;}

   virtual void do_initialize () throw (adt::RuntimeException) {;}
   virtual void do_initializer (Resource* resource) throw (adt::RuntimeException);
   virtual bool do_contains (const Resource* resource) const throw ();

   virtual Resource* do_apply (const int key) throw (adt::RuntimeException) = 0;

   bool requiresKey () const throw () { return m_requires != Requires::None; }
   bool requiresPositiveKey () const throw () { return m_requires == Requires::PositiveKey; }

private:
   mutable std::recursive_mutex m_mutex;
   const Requires::_v m_requires;
   resource_container m_resources;

   BalanceIf (const BalanceIf&);
};

} /* namespace balance */
} /* namespace wepa */
#endif
