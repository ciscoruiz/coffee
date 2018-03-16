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
   return adt::AsHexString::apply((int64_t) this);
}

adt::StreamString persistence::Object::asString() const noexcept
{
   adt::StreamString result("persistence.Object { ClassName=");
   result << m_class.getName();
   result << " | " << m_primaryKey->asString();
   
   result << " | Members={ ";
   bool first = true;
   for(auto& member : *this) {
      if (first == false) {
         result += ",";
      }
      result << member.second->asString();
      first = false;
   }
   return result << "} }";
}

