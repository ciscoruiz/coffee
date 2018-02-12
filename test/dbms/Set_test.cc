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

