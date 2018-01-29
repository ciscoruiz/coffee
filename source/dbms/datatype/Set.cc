// COFFEE - COmpany eFFEEctive Platform
//
//(c) Copyright 2018 Francisco Ruiz Rayo
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
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

void dbms::datatype::Set::insert(std::shared_ptr<Abstract> data) throw (adt::RuntimeException) {
   if (m_datas.find(data->getName()) != end()) {
      COFFEE_THROW_EXCEPTION(data->getName() << " already defined");
   }

   auto result = m_datas.insert(Datas::value_type(data->getName(), data));

   if (result.second) {
	   m_creationOrder.push_back(result.first);
   }
}

std::shared_ptr<dbms::datatype::Abstract>& dbms::datatype::Set::find(const std::string& name)
   throw (adt::RuntimeException)
{
   auto ii = m_datas.find(name);

   if (ii == end()) {
      COFFEE_THROW_EXCEPTION(name << " is not defined");
   }

   return data(ii);
}

const std::shared_ptr<dbms::datatype::Abstract>& dbms::datatype::Set::find(const std::string& name) const
   throw (adt::RuntimeException)
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
   throw (adt::RuntimeException)
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

   if (compareCounter != size() && result == 0) {
      COFFEE_THROW_EXCEPTION("Only " << compareCounter << " from " << size() << " components were compared");
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
   catch(adt::RuntimeException&) {
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
   const throw(adt::RuntimeException, dbms::InvalidDataException)
{
   auto member = coffee_datatype_downcast(dbms::datatype::String,find(columnName));
   return std::string(member->getValue());
}

float dbms::datatype::Set::getFloat(const std::string& columnName) const throw(dbms::InvalidDataException)
{
   auto member = coffee_datatype_downcast(dbms::datatype::Float,find(columnName));
   return member->getFloatValue();
}

const adt::DataBlock& dbms::datatype::Set::getDataBlock(const std::string& columnName) const throw(dbms::InvalidDataException)
{
   auto abstractMember = find(columnName);

   if(abstractMember->getType() == dbms::datatype::Abstract::Datatype::LongBlock)
      return coffee_datatype_downcast(dbms::datatype::LongBlock, abstractMember)->getValue();
   else
      return coffee_datatype_downcast(dbms::datatype::ShortBlock, abstractMember)->getValue();
}

const adt::Second& dbms::datatype::Set::getDate(const std::string& columnName) const throw(dbms::InvalidDataException)
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
   throw(adt::RuntimeException, dbms::InvalidDataException)
{
   auto member = coffee_datatype_downcast(dbms::datatype::String,find(columnName));
   member->setValue(value);
}

void dbms::datatype::Set::setFloat(const std::string& columnName, const float value) throw(dbms::InvalidDataException)
{
   auto member = coffee_datatype_downcast(dbms::datatype::Float,find(columnName));
   member->setValue(value);
}

void dbms::datatype::Set::setDataBlock(const std::string& columnName, const adt::DataBlock& value) throw(dbms::InvalidDataException)
{
   auto abstractMember = find(columnName);

   if(abstractMember->getType() == dbms::datatype::Abstract::Datatype::LongBlock)
      coffee_datatype_downcast(dbms::datatype::LongBlock, abstractMember)->setValue(value);
   else
      coffee_datatype_downcast(dbms::datatype::ShortBlock, abstractMember)->setValue(value);
}

void dbms::datatype::Set::setDate(const std::string& columnName, const adt::Second& value)
   throw(adt::RuntimeException, dbms::InvalidDataException)
{
   auto member = coffee_datatype_downcast(dbms::datatype::Date,find(columnName));
   member->setValue(value);
}
