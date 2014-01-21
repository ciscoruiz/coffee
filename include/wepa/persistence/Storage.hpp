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

#include <map>
#include <memory>

#include <auto_enum.h>

#include <wepa/adt/NamedObject.hpp>

#include <wepa/persistence/PrimaryKey.hpp>

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
class GuardClass;

class Storage : public adt::NamedObject {
   struct Entry {
      Object* m_object;
      unsigned int m_useCounter;

      Entry () : m_object (NULL), m_useCounter (0) {;}
      Entry (Object* object) : m_object (object), m_useCounter (1) {;}
      Entry (const Entry& other) : m_object (other.m_object), m_useCounter (other.m_useCounter) {;}
   };

   struct PtrLess {
      bool operator () (const PrimaryKey& first, const PrimaryKey& second) throw (adt::RuntimeException) {
         return first < second;
      }
   };

   typedef std::map <PrimaryKey, Entry, PtrLess> Entries;

public:
   class AccessMode {
   public:
      enum _v { None = -1, ReadOnly, ReadWrite, ReadAlways };
      auto_enum_declare (AccessMode);

   protected:
      Object* reload (dbms::Connection& connection, GuardClass& _class, Loader& loader, Entry& entry) const throw (adt::RuntimeException, dbms::DatabaseException);

   private:
      virtual Object* run (dbms::Connection& connection, GuardClass& _class, Loader& loader, Entry& entry) const throw (adt::RuntimeException, dbms::DatabaseException) = 0;

      friend class Storage;
   };

   typedef unsigned int Ident;

   ~Storage ();

   unsigned int getHitCounter () const noexcept { return m_hitCounter; }
   unsigned int getFaultCounter () const noexcept { return m_faultCounter; }

   Object& load (dbms::Connection& connection, GuardClass& _class, Loader& loader) throw (adt::RuntimeException, dbms::DatabaseException);

   operator adt::StreamString () const noexcept { return asString (); }

   adt::StreamString asString () const noexcept;

   xml::Node& asXML (xml::Node& parent) const noexcept;

   Storage () = delete;
   Storage (const Storage&) = delete;

protected:
   typedef Entries::iterator entry_iterator;
   typedef Entries::value_type entry_type;

   static Entry& entry (entry_type& ii) noexcept { return std::ref (ii.second); }
   static Object* object (entry_type& ii) noexcept { return ii.second.m_object; }

   static const PrimaryKey& primaryKey(entry_iterator ii) noexcept { return ii->first; }
   static Entry& entry (entry_iterator ii) noexcept { return entry (*ii); }
   static Object* object(entry_iterator ii) noexcept { return object (*ii); }

private:
   Entries m_objects;
   const AccessMode::_v m_accessMode;
   mutable unsigned int m_hitCounter;
   mutable unsigned int m_faultCounter;

   Storage (const char* name, const AccessMode::_v accessMode);

   class AccessModeReadOnly : public AccessMode {
      Object* run (dbms::Connection& connection, GuardClass& _class, Loader& loader, Entry& entry) const throw (adt::RuntimeException, dbms::DatabaseException);
   };

   class AccessModeReadWrite : public AccessMode {
      Object* run (dbms::Connection& connection, GuardClass& _class, Loader& loader, Entry& entry) const throw (adt::RuntimeException, dbms::DatabaseException);
   };

   class AccessModeReadAlways : public AccessMode {
      Object* run (dbms::Connection& connection, GuardClass& _class, Loader& loader, Entry& entry) const throw (adt::RuntimeException, dbms::DatabaseException);
   };

   static const AccessMode& instanciateAccessMode (const AccessMode::_v accessMode) throw (adt::RuntimeException);

   friend class Repository;
};

} /* namespace persistence */
} /* namespace wepa */
#endif
