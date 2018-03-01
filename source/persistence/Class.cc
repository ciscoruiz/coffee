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

#include <coffee/persistence/Class.hpp>
#include <coffee/persistence/ClassBuilder.hpp>
#include <coffee/persistence/Object.hpp>
#include <coffee/persistence/PrimaryKey.hpp>
#include <coffee/persistence/PrimaryKeyBuilder.hpp>

#include <coffee/logger/Logger.hpp>

#include <coffee/xml/Node.hpp>
#include <coffee/xml/Attribute.hpp>

#include <coffee/dbms/datatype/Abstract.hpp>

using namespace coffee;

persistence::Class::Class(const ClassBuilder& classBuilder) :
     adt::NamedObject(classBuilder.getClassName())

{
   m_primaryKey = classBuilder.getPrimaryKey();
   m_members = classBuilder;
}

std::shared_ptr<persistence::PrimaryKey> persistence::Class::createPrimaryKey() const
   throw ()
{
   PrimaryKeyBuilder builder;

   for (auto ii = m_primaryKey->begin(), maxii = m_primaryKey->end(); ii != maxii; ++ ii) {
      builder.add(PrimaryKey::data(ii)->clone());
   }

   return builder.build();
}

std::shared_ptr<persistence::Object> persistence::Class::createObject(const std::shared_ptr<PrimaryKey>& primaryKey) const
   throw (adt::RuntimeException)
{
   if (!m_primaryKey->matches(*primaryKey.get())) {
      COFFEE_THROW_EXCEPTION(asString() << " primary key does not matches with class");
   }

   dbms::datatype::Set members;

   for(dbms::datatype::Set::const_data_iterator ii = m_members.begin(), maxii = m_members.end(); ii != maxii; ++ ii) {
      members.insert(dbms::datatype::Set::data(ii)->clone());
   }

   return std::make_shared<Object>(*this, primaryKey, members);
}

adt::StreamString persistence::Class::asString() const noexcept {
   adt::StreamString result("persistence.Class { ");
   result << adt::NamedObject::asString();
   result << " | N-members=" << m_members.size();
   return result << " }";
}

std::shared_ptr<xml::Node> persistence::Class::asXML(std::shared_ptr<xml::Node>& parent) const noexcept
{
   std::shared_ptr<xml::Node> result = parent->createChild("persistence.Class");
   result->createAttribute("Name", this->getName());
   return result;
}

