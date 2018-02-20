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
