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
#include <wepa/persistence/Object.hpp>
#include <wepa/persistence/PrimaryKey.hpp>

#include <wepa/adt/AsHexString.hpp>

#include <wepa/persistence/Class.hpp>
#include <wepa/persistence/PrimaryKey.hpp>

#include <wepa/dbms/datatype/Date.hpp>
#include <wepa/dbms/datatype/Float.hpp>
#include <wepa/dbms/datatype/Integer.hpp>
#include <wepa/dbms/datatype/LongBlock.hpp>
#include <wepa/dbms/datatype/ShortBlock.hpp>
#include <wepa/dbms/datatype/String.hpp>

using namespace wepa;

persistence::Object::Object(const Class& clazz, const std::shared_ptr<PrimaryKey>& primaryKey, const Class::Members& members) :
   m_class(clazz), 
   m_primaryKey(primaryKey), 
   m_members(members)
{;}

std::string persistence::Object::getInternalId () const noexcept
{
   return adt::AsHexString::apply (wepa_ptrnumber_cast(this));
}

int persistence::Object::getInteger(const int columnNumber) const 
   throw(dbms::InvalidDataException) 
{ 
   auto member = wepa_datatype_downcast(dbms::datatype::Integer,getMember(columnNumber));
   return member->getValue();
}

std::string persistence::Object::getString(const int columnNumber) 
   const throw(adt::RuntimeException, dbms::InvalidDataException) 
{ 
   auto member = wepa_datatype_downcast(dbms::datatype::String,getMember(columnNumber));
   return std::string(member->getValue());
}

float persistence::Object::getFloat(const int columnNumber) const throw(dbms::InvalidDataException) 
{ 
   auto member = wepa_datatype_downcast(dbms::datatype::Float,getMember(columnNumber));
   return member->getFloatValue();
}

const adt::DataBlock& persistence::Object::getDataBlock(const int columnNumber) const throw(dbms::InvalidDataException) 
{ 
   auto abstractMember = getMember(columnNumber);
   
   if(abstractMember->getType() == dbms::datatype::Abstract::Datatype::LongBlock) 
      return wepa_datatype_downcast(dbms::datatype::LongBlock, abstractMember)->getValue();
   else
      return wepa_datatype_downcast(dbms::datatype::ShortBlock, abstractMember)->getValue();
}

const adt::Second& persistence::Object::getDate(const int columnNumber) const throw(dbms::InvalidDataException) 
{ 
   auto member = wepa_datatype_downcast(dbms::datatype::Date,getMember(columnNumber));
   return member->getValue();   
}

void persistence::Object::setInteger(const int columnNumber, const int value) throw(dbms::InvalidDataException) 
{ 
   auto member = wepa_datatype_downcast(dbms::datatype::Integer,getMember(columnNumber));
   member->setValue(value);
}

void persistence::Object::setString(const int columnNumber, const std::string& value) 
   throw(adt::RuntimeException, dbms::InvalidDataException) 
{ 
   auto member = wepa_datatype_downcast(dbms::datatype::String,getMember(columnNumber));
   member->setValue(value);   
}

void persistence::Object::setFloat(const int columnNumber, const float value) throw(dbms::InvalidDataException) 
{ 
   auto member = wepa_datatype_downcast(dbms::datatype::Float,getMember(columnNumber));
   member->setValue(value);
}

void persistence::Object::setDataBlock(const int columnNumber, const adt::DataBlock& value) throw(dbms::InvalidDataException) 
{ 
   auto abstractMember = getMember(columnNumber);

   if(abstractMember->getType() == dbms::datatype::Abstract::Datatype::LongBlock) 
      wepa_datatype_downcast(dbms::datatype::LongBlock, abstractMember)->setValue(value);
   else
      wepa_datatype_downcast(dbms::datatype::ShortBlock, abstractMember)->setValue(value);   
}

void persistence::Object::setDate(const int columnNumber, const adt::Second& value) 
   throw(adt::RuntimeException, dbms::InvalidDataException) 
{ 
   auto member = wepa_datatype_downcast(dbms::datatype::Date,getMember(columnNumber));
   member->setValue(value);   
}

std::shared_ptr<dbms::datatype::Abstract>& persistence::Object::getMember(const int columnNumber)
   throw(adt::RuntimeException)
{
   if(columnNumber >= m_members.size()) {
      WEPA_THROW_EXCEPTION(asString() << " | Column=" << columnNumber << " is out range");
   }

   return std::ref(m_members[columnNumber].first);
}

const std::shared_ptr<dbms::datatype::Abstract>& persistence::Object::getMember(const int columnNumber) const
   throw(adt::RuntimeException)
{
   if(columnNumber >= m_members.size()) {
      WEPA_THROW_EXCEPTION(asString() << " | Column=" << columnNumber << " is out range");
   }

   return std::ref(m_members[columnNumber].first);
}

adt::StreamString persistence::Object::asString() const noexcept
{
   adt::StreamString result("persistence.Object { ClassName=");
   result << m_class.getName();
   result << " | " << m_primaryKey->asString();
   
   result << " | Members={";
   for(auto& member : m_members) {
      if(!member.second) {
         result << "," << member.first->asString();
      }
   }
   return result << "} }";
}

