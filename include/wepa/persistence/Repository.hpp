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
#ifndef __wepa_persistence_Repository_hpp
#define __wepa_persistence_Repository_hpp

#include <boost/ptr_container/ptr_map.hpp>

#include <wepa/adt/NamedObject.hpp>
#include <wepa/adt/RuntimeException.hpp>

#include <wepa/persistence/Storage.hpp>

namespace wepa {

namespace xml {
class Node;
}

namespace persistence {

class Repository : public adt::NamedObject {
public:
   explicit Repository (const char* name) : NamedObject(name) {;}
   explicit Repository (const std::string& name) : NamedObject(name) {;}

   Storage* createStorage (const Storage::Ident ident, const char* name, const Storage::AccessMode::_v accessMode, Storage::ObjectFactory factory)
      throw (adt::RuntimeException);

   Storage& findStorage (const Storage::Ident ident) throw (adt::RuntimeException);

   adt::StreamString asString () const noexcept;

   xml::Node& asXML (xml::Node& parent) const noexcept;

   Repository (const Repository&) = delete;

private:
   typedef boost::ptr_map <Storage::Ident, Storage> Storages;
   typedef Storages::iterator storage_iterator;
   typedef Storages::const_iterator const_storage_iterator;

   Storages m_storages;

   storage_iterator storage_begin () noexcept { return m_storages.begin (); }
   storage_iterator storage_end () noexcept { return m_storages.end (); }
   static Storage& storage (storage_iterator ii) noexcept { return std::ref (*(ii->second)); }

   const_storage_iterator storage_begin () const noexcept { return m_storages.begin (); }
   const_storage_iterator storage_end () const noexcept { return m_storages.end (); }
   static const Storage& storage (const_storage_iterator ii) noexcept { return std::ref (*(ii->second)); }
};

} /* namespace persistence */
} /* namespace wepa */
#endif
