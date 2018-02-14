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
#include <boost/test/unit_test.hpp>

#include <coffee/persistence/ClassBuilder.hpp>
#include <coffee/persistence/PrimaryKeyBuilder.hpp>
#include <coffee/persistence/PrimaryKey.hpp>
#include <coffee/persistence/Class.hpp>
#include <coffee/persistence/Object.hpp>

#include <coffee/dbms/datatype/Integer.hpp>
#include <coffee/dbms/datatype/String.hpp>
#include <coffee/dbms/datatype/Date.hpp>
#include <coffee/dbms/datatype/Float.hpp>

using namespace coffee;

BOOST_AUTO_TEST_CASE(persistence_class)
{
   persistence::PrimaryKeyBuilder pkBuilder;
   pkBuilder.add(std::make_shared<dbms::datatype::Integer>("id")).add(std::make_shared<dbms::datatype::String>("id2", 64));
   std::shared_ptr<persistence::PrimaryKey> fixedPrimaryKey = pkBuilder.build();

   persistence::ClassBuilder classBuilder("the-class");
   classBuilder.set(fixedPrimaryKey).add(std::make_shared<dbms::datatype::String>("name", 64));
   classBuilder.add(std::make_shared<dbms::datatype::Date>("date"));
   classBuilder.add(std::make_shared<dbms::datatype::Float>("the-float"));

   std::shared_ptr<persistence::Class> clazz = classBuilder.build();

   auto pkObject = clazz->createPrimaryKey();
   BOOST_REQUIRE_NO_THROW(pkObject->setInteger("id", 111));
   BOOST_REQUIRE_NO_THROW(pkObject->setString("id2", "zzz"));

   auto object = clazz->createObject(pkObject);

   BOOST_REQUIRE_EQUAL(object->getPrimaryKey()->getInteger("id"), 111);
   BOOST_REQUIRE_EQUAL(object->getPrimaryKey()->getString("id2"), "zzz");

   const adt::Second now = adt::Second::getLocalTime();

   BOOST_REQUIRE_NO_THROW(object->setString("name", "the name"));
   BOOST_REQUIRE_NO_THROW(object->setDate("date", now));
   BOOST_REQUIRE_NO_THROW(object->setFloat("the-float", 1111.22));

   BOOST_REQUIRE_EQUAL(object->getString("name"), "the name");
   BOOST_REQUIRE_EQUAL(object->getDate("date"), now);
   BOOST_REQUIRE_CLOSE(object->getFloat("the-float"), 1111.22, 0.1);

   auto object2 = clazz->createObject(pkObject);

   BOOST_REQUIRE(object->getInternalId() != object2->getInternalId());
}
