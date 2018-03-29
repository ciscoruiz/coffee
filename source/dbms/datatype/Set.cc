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

#include <utility>

#include <coffee/dbms/datatype/Set.hpp>

#include <coffee/dbms/datatype/Date.hpp>
#include <coffee/dbms/datatype/Float.hpp>
#include <coffee/dbms/datatype/Integer.hpp>
#include <coffee/dbms/datatype/LongBlock.hpp>
#include <coffee/dbms/datatype/ShortBlock.hpp>
#include <coffee/dbms/datatype/String.hpp>

using namespace coffee;

dbms::datatype::Set::Set(const Set& other)
{
   for (auto ii : other.m_creationOrder) {
      insert(data(ii));
   }
}

void dbms::datatype::Set::insert(std::shared_ptr<Abstract> data) throw (basis::RuntimeException) {
   if (!data) {
      COFFEE_THROW_EXCEPTION("data must be initialized");
   }

   if (m_datas.find(data->getName()) != end()) {
      COFFEE_THROW_EXCEPTION(data->getName() << " already defined");
   }

   auto result = m_datas.insert(Datas::value_type(data->getName(), data));

   if (result.second) {
      m_creationOrder.push_back(result.first);
   }
}

std::shared_ptr<dbms::datatype::Abstract>& dbms::datatype::Set::find(const std::string& name)
   throw (basis::RuntimeException)
{
   auto ii = m_datas.find(name);

   if (ii == end()) {
      COFFEE_THROW_EXCEPTION(name << " is not defined");
   }

   return data(ii);
}

const std::shared_ptr<dbms::datatype::Abstract>& dbms::datatype::Set::find(const std::string& name) const
   throw (basis::RuntimeException)
{
   auto ii = m_datas.find(name);

   if (ii == end()) {
      COFFEE_THROW_EXCEPTION(name << " is not defined");
   }

   return data(ii);
}

dbms::datatype::Set& dbms::datatype::Set::operator=(const Set& other)
   noexcept
{
   if (this == &other) {
      return *this;
   }

   m_datas.clear();
   m_creationOrder.clear();
   for (auto ii : other.m_creationOrder) {
      insert(data(ii));
   }

   return *this;
}

int dbms::datatype::Set::compare(const Set& other) const
   throw (basis::RuntimeException)
{
   if (this == &other) {
      return 0;
   }

   if (size() != other.size()) {
      COFFEE_THROW_EXCEPTION(
         "Can not compare sets with different number of components | N-Size=" << size() << " | other-Size=" << other.size()
      );
   }

   int compareCounter = 0;
   int result = 0;

   for (auto ii : m_creationOrder) {
      ++ compareCounter;

      result = Set::data(ii)->compare(other.find(Set::name(ii)));

      if (result != 0)
         break;
   }

   return result;
}

bool dbms::datatype::Set::operator==(const Set& other) const
   noexcept
{
   bool result;

   try {
      result = (compare(other) == 0);
   }
   catch(basis::RuntimeException&) {
      result = false;
   }

   return result;
}

int dbms::datatype::Set::getInteger(const std::string& columnName) const
   throw(dbms::InvalidDataException)
{
   auto member = coffee_datatype_downcast(dbms::datatype::Integer,find(columnName));
   return member->getValue();
}

std::string dbms::datatype::Set::getString(const std::string& columnName)
   const throw(basis::RuntimeException, dbms::InvalidDataException)
{
   auto member = coffee_datatype_downcast(dbms::datatype::String,find(columnName));
   return std::string(member->getValue());
}

float dbms::datatype::Set::getFloat(const std::string& columnName) const throw(dbms::InvalidDataException)
{
   auto member = coffee_datatype_downcast(dbms::datatype::Float,find(columnName));
   return member->getFloatValue();
}

const basis::DataBlock& dbms::datatype::Set::getDataBlock(const std::string& columnName) const throw(dbms::InvalidDataException)
{
   auto abstractMember = find(columnName);

   if(abstractMember->getType() == dbms::datatype::Abstract::Datatype::LongBlock)
      return coffee_datatype_downcast(dbms::datatype::LongBlock, abstractMember)->getValue();
   else
      return coffee_datatype_downcast(dbms::datatype::ShortBlock, abstractMember)->getValue();
}

const std::chrono::seconds& dbms::datatype::Set::getDate(const std::string& columnName) const throw(dbms::InvalidDataException)
{
   auto member = coffee_datatype_downcast(dbms::datatype::Date,find(columnName));
   return member->getValue();
}

void dbms::datatype::Set::setInteger(const std::string& columnName, const int value) throw(dbms::InvalidDataException)
{
   auto member = coffee_datatype_downcast(dbms::datatype::Integer,find(columnName));
   member->setValue(value);
}

void dbms::datatype::Set::setString(const std::string& columnName, const std::string& value)
   throw(basis::RuntimeException, dbms::InvalidDataException)
{
   auto member = coffee_datatype_downcast(dbms::datatype::String,find(columnName));
   member->setValue(value);
}

void dbms::datatype::Set::setFloat(const std::string& columnName, const float value) throw(dbms::InvalidDataException)
{
   auto member = coffee_datatype_downcast(dbms::datatype::Float,find(columnName));
   member->setValue(value);
}

void dbms::datatype::Set::setDataBlock(const std::string& columnName, const basis::DataBlock& value) throw(dbms::InvalidDataException)
{
   auto abstractMember = find(columnName);

   if(abstractMember->getType() == dbms::datatype::Abstract::Datatype::LongBlock)
      coffee_datatype_downcast(dbms::datatype::LongBlock, abstractMember)->setValue(value);
   else
      coffee_datatype_downcast(dbms::datatype::ShortBlock, abstractMember)->setValue(value);
}

void dbms::datatype::Set::setDate(const std::string& columnName, const std::chrono::seconds& value)
   throw(basis::RuntimeException, dbms::InvalidDataException)
{
   auto member = coffee_datatype_downcast(dbms::datatype::Date,find(columnName));
   member->setValue(value);
}
