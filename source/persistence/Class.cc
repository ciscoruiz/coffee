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
#include <wepa/persistence/Class.hpp>

#include <wepa/logger/Logger.hpp>

#include <wepa/xml/Node.hpp>
#include <wepa/xml/Attribute.hpp>

#include <wepa/dbms/datatype/Abstract.hpp>

using namespace wepa;

persistence::Class::~Class ()
{
   m_members.clear ();
}

void persistence::Class::createMembers ()
   throw (adt::RuntimeException)
{
   dbms::datatype::Abstract* member;

   int columnNumber = 0;
   while ((member = do_createMember (columnNumber ++)) != NULL) {
      m_members.push_back (member);
   }

   LOG_DEBUG (asString ());

   if (m_members.size () == 0) {
      WEPA_THROW_EXCEPTION(asString () << " does not define any member");
   }
}

dbms::datatype::Abstract& persistence::Class::getMember (const int columnNumber)
   throw (adt::RuntimeException)
{
   if (columnNumber >= m_members.size ()) {
      WEPA_THROW_EXCEPTION(asString () << " | Column=" << columnNumber << " is out range");
   }

   return std::ref (m_members [columnNumber]);
}

const dbms::datatype::Abstract& persistence::Class::getMember (const int columnNumber) const
   throw (adt::RuntimeException)
{
   if (columnNumber >= m_members.size ()) {
      WEPA_THROW_EXCEPTION(asString () << " | Column=" << columnNumber << " is out range");
   }

   return std::ref (m_members [columnNumber]);
}

adt::StreamString persistence::Class::asString () const noexcept {
   adt::StreamString result ("persistence.Class { ");
   result << adt::NamedObject::asString();
   result << " | N-members=" << m_members.size ();
   return result << " }";
}

xml::Node& persistence::Class::asXML (xml::Node& parent) const noexcept
{
   xml::Node& result = parent.createChild("persistence.Class");
   result.createAttribute("Name", this->getName ());
   return result;
}

