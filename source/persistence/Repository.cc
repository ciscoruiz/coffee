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
#include <wepa/persistence/Repository.hpp>

#include <wepa/logger/Logger.hpp>

#include <wepa/xml/Node.hpp>
#include <wepa/xml/Attribute.hpp>

using namespace wepa;

persistence::Storage* persistence::Repository::createStorage (const Storage::Ident ident, const char* name, const Storage::AccessMode::_v accessMode, Storage::ObjectFactory factory)
   throw (adt::RuntimeException)
{
   persistence::Storage* result;

   if (accessMode == Storage::AccessMode::None) {
      WEPA_THROW_EXCEPTION ("Ident=" << ident << " must indicate a valid access mode");
   }

   storage_iterator ii = m_storages.find (ident);

   if (ii != m_storages.end()) {
      WEPA_THROW_EXCEPTION("Ident=" << ident << " has already been used");
   }

   result = new Storage (name, accessMode, factory);

   LOG_DEBUG(result->asString ());

   Storage::Ident lvalue = ident;
   m_storages.insert (lvalue, result);

   return result;
}

persistence::Storage& persistence::Repository::findStorage (const Storage::Ident ident)
   throw (adt::RuntimeException)
{
   storage_iterator ii = m_storages.find (ident);

   if (ii == m_storages.end()) {
      WEPA_THROW_EXCEPTION("ID=" << ident << " has not been defined yet");
   }

   return std::ref (storage (ii));
}

adt::StreamString persistence::Repository::asString () const
   noexcept
{
   adt::StreamString result ("persistence.Repository { ");
   result << adt::NamedObject::asString();
   result << " | N-Size=" << m_storages.size ();
   return result += " }";
}

xml::Node& persistence::Repository::asXML (xml::Node& parent) const
   noexcept
{
   xml::Node& result = parent.createChild("persistence.Repository");

   result.createAttribute("Name", getName ());

   for (const_storage_iterator ii = storage_begin(), maxii = storage_end(); ii != maxii; ++ ii)  {
      storage (ii).asXML(result);
   }

   return result;
}
