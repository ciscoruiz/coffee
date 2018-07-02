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

#include <coffee/basis/DataBlock.hpp>
#include <coffee/basis/AsString.hpp>

#include <coffee/time/TimeService.hpp>

#include <coffee/dbms/datatype/Set.hpp>
#include <coffee/dbms/datatype/Integer.hpp>
#include <coffee/dbms/datatype/String.hpp>
#include <coffee/dbms/datatype/Float.hpp>
#include <coffee/dbms/datatype/Date.hpp>
#include <coffee/dbms/datatype/LongBlock.hpp>

#include <gtest/gtest.h>

using namespace coffee;
using namespace coffee::dbms;

TEST(SetTest,already_defined)
{
  datatype::Set set;
  set.insert(datatype::Integer::instantiate("integer"));
  ASSERT_THROW(set.insert(datatype::Integer::instantiate("integer")), basis::RuntimeException);
}

TEST(SetTest,empty_field)
{
  datatype::Set set;
  std::shared_ptr<datatype::Integer> data;
  ASSERT_THROW(set.insert(data), basis::RuntimeException);
}

TEST(SetTest,find)
{
  datatype::Set set;

  set.insert(datatype::Integer::instantiate("integer"));

  ASSERT_NO_THROW(set.find("integer"));

  ASSERT_THROW(set.find("other-name"), basis::RuntimeException);
}

TEST(SetTest,constains)
{
  datatype::Set set;

  set.insert(datatype::Integer::instantiate("integer"));

  ASSERT_TRUE(set.constains("integer"));
  ASSERT_FALSE(set.constains("integer2"));
}

TEST(SetTest,access)
{
   datatype::Set set;

   std::chrono::seconds second = time::TimeService::toSeconds(time::TimeService::now());

   set.insert(datatype::Integer::instantiate("integer"));
   set.insert(datatype::String::instantiate("string", 16));
   set.insert(datatype::Float::instantiate("float"));
   set.insert(datatype::Date::instantiate("date"));
   set.insert(datatype::LongBlock::instantiate("longblock"));

   const int maxSize = 1024;
   char* buffer = new char[maxSize];
   basis::DataBlock data(buffer, maxSize);

   set.setInteger("integer", 123);
   set.setString("string", "hello");
   set.setFloat("float", 0.123);
   set.setDate("date", second);
   set.setDataBlock("longblock", data);

   ASSERT_EQ(123, set.getInteger("integer"));
   ASSERT_EQ("hello", set.getString("string"));
   ASSERT_FLOAT_EQ(0.123, set.getFloat("float"));
   ASSERT_EQ(second, set.getDate("date"));

   const basis::DataBlock& read = set.getDataBlock("longblock");

   ASSERT_TRUE(memcmp(read.data(), buffer, maxSize) == 0);
}

TEST(SetTest,operator_compare)
{
  datatype::Set set;

  set.insert(datatype::Integer::instantiate("integer"));
  set.insert(datatype::String::instantiate("string", 16));
  set.insert(datatype::Float::instantiate("float"));
  set.setInteger("integer", 123);
  set.setString("string", "hello");
  set.setFloat("float", 0.123);

  datatype::Set other;

  other.insert(datatype::Integer::instantiate("integer"));
  other.insert(datatype::String::instantiate("string", 16));
  other.insert(datatype::Float::instantiate("float"));
  other.setInteger("integer", 123);
  other.setString("string", "hello");
  other.setFloat("float", 0.123);

  ASSERT_TRUE(set == other);
  ASSERT_FALSE(set < other);
}

TEST(SetTest,operator_less)
{
  datatype::Set set;

  set.insert(datatype::Integer::instantiate("integer"));
  set.insert(datatype::String::instantiate("string", 16));
  set.insert(datatype::Float::instantiate("float"));
  set.setInteger("integer", 123);
  set.setString("string", "hello");
  set.setFloat("float", 0.123);

  datatype::Set other;

  other.insert(datatype::Integer::instantiate("integer"));
  other.insert(datatype::String::instantiate("string", 16));
  other.insert(datatype::Float::instantiate("float"));
  other.setInteger("integer", 123);
  other.setString("string", "hello");
  other.setFloat("float", 0.0);

  ASSERT_FALSE(set < other);
  ASSERT_TRUE(other < set);
}

TEST(SetTest,self)
{
  datatype::Set set;

  set = set;

  set.insert(datatype::Integer::instantiate("integer"));
  set.insert(datatype::String::instantiate("string", 16));
  set.insert(datatype::Float::instantiate("float"));
  set.setInteger("integer", 123);
  set.setString("string", "hello");
  set.setFloat("float", 0.123);

  ASSERT_TRUE(set == set);
}

TEST(SetTest,compare_different_no_members_numbers)
{
  datatype::Set set;

  set.insert(datatype::Integer::instantiate("integer"));
  set.insert(datatype::String::instantiate("string", 16));
  set.insert(datatype::Float::instantiate("float"));
  set.setInteger("integer", 123);
  set.setString("string", "hello");
  set.setFloat("float", 0.123);

  datatype::Set other;
  other.insert(datatype::Integer::instantiate("integer"));
  other.insert(datatype::String::instantiate("string", 16));
  set.setInteger("integer", 123);
  set.setString("string", "hello");

  ASSERT_THROW(set.compare(other), basis::RuntimeException);
  ASSERT_TRUE(set != other);

  ASSERT_THROW(other.compare(set), basis::RuntimeException);
  ASSERT_TRUE(other != set);
}

TEST(SetTest,compare_different_members_names)
{
  datatype::Set set;

  set.insert(datatype::Integer::instantiate("integer"));
  set.insert(datatype::String::instantiate("string", 16));
  set.insert(datatype::Float::instantiate("float"));
  set.setInteger("integer", 123);
  set.setString("string", "hello");
  set.setFloat("float", 0.123);

  datatype::Set other;
  other.insert(datatype::Integer::instantiate("integer"));
  other.insert(datatype::String::instantiate("string", 16));
  other.insert(datatype::Float::instantiate("float-with-other-name"));
  other.setInteger("integer", 123);
  other.setString("string", "hello");
  other.setFloat("float-with-other-name", 0.123);

  ASSERT_THROW(set.compare(other), basis::RuntimeException);
  ASSERT_THROW(other.compare(set), basis::RuntimeException);
  ASSERT_TRUE(set != other);
  ASSERT_TRUE(other != set);
}



