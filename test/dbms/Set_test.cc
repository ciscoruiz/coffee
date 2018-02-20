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

#include <coffee/adt/Second.hpp>
#include <coffee/adt/DataBlock.hpp>

#include <coffee/dbms/datatype/Set.hpp>
#include <coffee/dbms/datatype/Integer.hpp>
#include <coffee/dbms/datatype/String.hpp>
#include <coffee/dbms/datatype/Float.hpp>
#include <coffee/dbms/datatype/Date.hpp>
#include <coffee/dbms/datatype/LongBlock.hpp>

#include <boost/test/unit_test.hpp>

using namespace coffee;
using namespace coffee::dbms;

BOOST_AUTO_TEST_CASE(set_already_defined)
{
  datatype::Set set;
  set.insert(std::make_shared<datatype::Integer>("integer"));
  BOOST_REQUIRE_THROW(set.insert(std::make_shared<datatype::Integer>("integer")), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(set_empty_field)
{
  datatype::Set set;
  std::shared_ptr<datatype::Integer> data;
  BOOST_REQUIRE_THROW(set.insert(data), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(set_find)
{
  datatype::Set set;

  set.insert(std::make_shared<datatype::Integer>("integer"));

  BOOST_REQUIRE_NO_THROW(set.find("integer"));

  BOOST_REQUIRE_THROW(set.find("other-name"), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(set_constains)
{
  datatype::Set set;

  set.insert(std::make_shared<datatype::Integer>("integer"));

  BOOST_REQUIRE_EQUAL(set.constains("integer"), true);
  BOOST_REQUIRE_EQUAL(set.constains("integer2"), false);
}

BOOST_AUTO_TEST_CASE(set_access)
{
   datatype::Set set;

   adt::Second second = adt::Second::getLocalTime();

   set.insert(std::make_shared<datatype::Integer>("integer"));
   set.insert(std::make_shared<datatype::String>("string", 16));
   set.insert(std::make_shared<datatype::Float>("float"));
   set.insert(std::make_shared<datatype::Date>("date"));
   set.insert(std::make_shared<datatype::LongBlock>("longblock"));

   const int maxSize = 1024;
   char* buffer = new char[maxSize];
   adt::DataBlock data(buffer, maxSize);

   set.setInteger("integer", 123);
   set.setString("string", "hello");
   set.setFloat("float", 0.123);
   set.setDate("date", second);
   set.setDataBlock("longblock", data);

   BOOST_REQUIRE_EQUAL(set.getInteger("integer"), 123);
   BOOST_REQUIRE_EQUAL(set.getString("string"), "hello");
   BOOST_REQUIRE_CLOSE(set.getFloat("float"), 0.123, 0.001);
   BOOST_REQUIRE_EQUAL(set.getDate("date"), second);

   const adt::DataBlock& read = set.getDataBlock("longblock");

   BOOST_REQUIRE (memcmp(read.data(), buffer, maxSize) == 0);
}

BOOST_AUTO_TEST_CASE(set_operator_compare)
{
  datatype::Set set;

  set.insert(std::make_shared<datatype::Integer>("integer"));
  set.insert(std::make_shared<datatype::String>("string", 16));
  set.insert(std::make_shared<datatype::Float>("float"));
  set.setInteger("integer", 123);
  set.setString("string", "hello");
  set.setFloat("float", 0.123);

  datatype::Set other;

  other.insert(std::make_shared<datatype::Integer>("integer"));
  other.insert(std::make_shared<datatype::String>("string", 16));
  other.insert(std::make_shared<datatype::Float>("float"));
  other.setInteger("integer", 123);
  other.setString("string", "hello");
  other.setFloat("float", 0.123);

  BOOST_REQUIRE_EQUAL(set == other, true);
  BOOST_REQUIRE_EQUAL(set < other, false);
}

BOOST_AUTO_TEST_CASE(set_operator_less)
{
  datatype::Set set;

  set.insert(std::make_shared<datatype::Integer>("integer"));
  set.insert(std::make_shared<datatype::String>("string", 16));
  set.insert(std::make_shared<datatype::Float>("float"));
  set.setInteger("integer", 123);
  set.setString("string", "hello");
  set.setFloat("float", 0.123);

  datatype::Set other;

  other.insert(std::make_shared<datatype::Integer>("integer"));
  other.insert(std::make_shared<datatype::String>("string", 16));
  other.insert(std::make_shared<datatype::Float>("float"));
  other.setInteger("integer", 123);
  other.setString("string", "hello");
  other.setFloat("float", 0.0);

  BOOST_REQUIRE_EQUAL(set < other, false);
  BOOST_REQUIRE_EQUAL(other < set, true);
}

BOOST_AUTO_TEST_CASE(set_self)
{
  datatype::Set set;

  set = set;

  set.insert(std::make_shared<datatype::Integer>("integer"));
  set.insert(std::make_shared<datatype::String>("string", 16));
  set.insert(std::make_shared<datatype::Float>("float"));
  set.setInteger("integer", 123);
  set.setString("string", "hello");
  set.setFloat("float", 0.123);

  BOOST_REQUIRE(set == set);
}

BOOST_AUTO_TEST_CASE(set_compare_different_no_members_numbers)
{
  datatype::Set set;

  set.insert(std::make_shared<datatype::Integer>("integer"));
  set.insert(std::make_shared<datatype::String>("string", 16));
  set.insert(std::make_shared<datatype::Float>("float"));
  set.setInteger("integer", 123);
  set.setString("string", "hello");
  set.setFloat("float", 0.123);

  datatype::Set other;
  other.insert(std::make_shared<datatype::Integer>("integer"));
  other.insert(std::make_shared<datatype::String>("string", 16));
  set.setInteger("integer", 123);
  set.setString("string", "hello");

  BOOST_REQUIRE_THROW(set.compare(other), adt::RuntimeException);
  BOOST_REQUIRE_EQUAL(set == other, false);

  BOOST_REQUIRE_THROW(other.compare(set), adt::RuntimeException);
  BOOST_REQUIRE_EQUAL(other == set, false);

}

BOOST_AUTO_TEST_CASE(set_compare_different_members_names)
{
  datatype::Set set;

  set.insert(std::make_shared<datatype::Integer>("integer"));
  set.insert(std::make_shared<datatype::String>("string", 16));
  set.insert(std::make_shared<datatype::Float>("float"));
  set.setInteger("integer", 123);
  set.setString("string", "hello");
  set.setFloat("float", 0.123);

  datatype::Set other;
  other.insert(std::make_shared<datatype::Integer>("integer"));
  other.insert(std::make_shared<datatype::String>("string", 16));
  other.insert(std::make_shared<datatype::Float>("float2"));
  other.setInteger("integer", 123);
  other.setString("string", "hello");
  other.setFloat("float2", 0.123);

  BOOST_REQUIRE_THROW(set.compare(other), adt::RuntimeException);
  BOOST_REQUIRE_THROW(other.compare(set), adt::RuntimeException);
  BOOST_REQUIRE_EQUAL(set == other, false);
  BOOST_REQUIRE_EQUAL(other == set, false);
}

