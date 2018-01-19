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
#ifndef __wepa_persistence_Storage_hpp
#define __wepa_persistence_Storage_hpp

#include <memory>

#include <wepa/adt/NamedObject.hpp>

#include <wepa/adt/pattern/lru/Cache.hpp>

#include <wepa/persistence/PrimaryKey.hpp>
#include <wepa/persistence/Accessor.hpp>

#include <wepa/dbms/DatabaseException.hpp>

namespace wepa {
   namespace xml {
      class Node;
   }
   namespace dbms {
      class Connection;
   }

namespace persistence {

class Repository;
class Object;
class Loader;
class Recorder;
class Eraser;
class Creator;

class Storage : public adt::NamedObject {
public:

private:
   typedef adt::pattern::lru::Cache<Accessor::ThePrimaryKey, Accessor::TheObject, persistence::PrimaryKey::HashSharedPointer, persistence::PrimaryKey::EqualSharedPointer> Cache;

   static const int UpperLimitForMaxCacheSize = 4 * 1024;
   static const int LowerLimitForMaxCacheSize = 16;

public:

   static const int DefaultMaxCacheSize = 128;

   Storage (const std::string& name, const int maxCacheSize);
   ~Storage ();

   unsigned int getHitCounter () const noexcept { return m_hitCounter; }
   unsigned int getFaultCounter () const noexcept { return m_faultCounter; }

   Accessor::TheObject load (Accessor::TheConnection& connection, Loader& loader)
      throw (adt::RuntimeException, dbms::DatabaseException);
   void save (Accessor::TheConnection& connection, Recorder& recorder)
      throw (adt::RuntimeException, dbms::DatabaseException);
   void erase (Accessor::TheConnection& connection, Eraser& eraser)
      throw (adt::RuntimeException, dbms::DatabaseException);

   operator adt::StreamString () const noexcept { return asString (); }

   adt::StreamString asString () const noexcept;

   xml::Node& asXML (xml::Node& parent) const noexcept;

   Storage () = delete;
   Storage (const Storage&) = delete;
   
private:
   Cache m_cache;
   mutable unsigned int m_hitCounter;
   mutable unsigned int m_faultCounter;
};

} /* namespace persistence */
} /* namespace wepa */
#endif
