// COFFEE - COmpany eFFEEctive Platform
//
// (c) Copyright 2018 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/coffee
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
#include <coffee/persistence/Object.hpp>
#include <coffee/persistence/PrimaryKey.hpp>

#include <coffee/adt/AsHexString.hpp>

#include <coffee/persistence/Class.hpp>
#include <coffee/persistence/PrimaryKey.hpp>


using namespace coffee;

persistence::Object::Object(const Class& clazz, const std::shared_ptr<PrimaryKey>& primaryKey, const dbms::datatype::Set& members) :
   dbms::datatype::Set(members),
   m_class(clazz), 
   m_primaryKey(primaryKey)
{;}

std::string persistence::Object::getInternalId () const noexcept
{
   return adt::AsHexString::apply (coffee_ptrnumber_cast(this));
}

adt::StreamString persistence::Object::asString() const noexcept
{
   adt::StreamString result("persistence.Object { ClassName=");
   result << m_class.getName();
   result << " | " << m_primaryKey->asString();
   
   result << " | Members={";
   for(auto& member : *this) {
      if(!member.second) {
         result << "," << member.second->getName();
      }
   }
   return result << "} }";
}

