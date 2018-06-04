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

#include <gtest/gtest.h>

#include <coffee/persistence/ClassBuilder.hpp>
#include <coffee/persistence/PrimaryKeyBuilder.hpp>
#include <coffee/persistence/PrimaryKey.hpp>
#include <coffee/persistence/Class.hpp>
#include <coffee/persistence/Object.hpp>

#include <coffee/time/TimeService.hpp>

#include <coffee/dbms/datatype/Integer.hpp>
#include <coffee/dbms/datatype/String.hpp>
#include <coffee/dbms/datatype/Date.hpp>
#include <coffee/dbms/datatype/Float.hpp>

using namespace coffee;

TEST(ClassTest, clazz)
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
   ASSERT_NO_THROW(pkObject->setInteger("id", 111));
   ASSERT_NO_THROW(pkObject->setString("id2", "zzz"));

   auto object = clazz->createObject(pkObject);

   ASSERT_EQ(111, object->getPrimaryKey()->getInteger("id"));
   ASSERT_EQ("zzz", object->getPrimaryKey()->getString("id2"));

   auto now = time::TimeService::toSeconds(time::TimeService::now());

   ASSERT_NO_THROW(object->setString("name", "the name"));
   ASSERT_NO_THROW(object->setDate("date", now));
   ASSERT_NO_THROW(object->setFloat("the-float", 1111.22));

   ASSERT_EQ("the name", object->getString("name"));
   ASSERT_EQ(now, object->getDate("date"));
   ASSERT_FLOAT_EQ(object->getFloat("the-float"), 1111.22);

   auto object2 = clazz->createObject(pkObject);

   ASSERT_TRUE(object->getInternalId() != object2->getInternalId());
}

TEST(ClassTest, object_asstring)
{
   persistence::PrimaryKeyBuilder pkBuilder;
   pkBuilder.add(std::make_shared<dbms::datatype::Integer>("id")).add(std::make_shared<dbms::datatype::String>("id2", 64));
   std::shared_ptr<persistence::PrimaryKey> fixedPrimaryKey = pkBuilder.build();

   persistence::ClassBuilder classBuilder("the-class");
   classBuilder.set(fixedPrimaryKey).add(std::make_shared<dbms::datatype::String>("name", 64));
   classBuilder.add(std::make_shared<dbms::datatype::Date>("date"));
   classBuilder.add(std::make_shared<dbms::datatype::Float>("the-float", dbms::datatype::Constraint::CanBeNull));

   std::shared_ptr<persistence::Class> clazz = classBuilder.build();

   auto pkObject = clazz->createPrimaryKey();
   ASSERT_NO_THROW(pkObject->setInteger("id", 111));
   ASSERT_NO_THROW(pkObject->setString("id2", "zzz"));

   auto object = clazz->createObject(pkObject);

   const char* lines[] = {
      "persistence.Object { ClassName=the-class | persistence.PrimaryKey { ",
      "datatype.Integer { datatype.Abstract { Name=id | IsNull=false | Constraint=CanNotBeNull } | Value=111 }",
      "datatype.String { datatype.Abstract { Name=id2 | IsNull=false | Constraint=CanNotBeNull } | MaxSize=64 | Value='zzz' }",
      "Members={ ",
      "datatype.String { datatype.Abstract { Name=name | IsNull=false | Constraint=CanNotBeNull } | MaxSize=64 | Value='' }",
      "datatype.Float { datatype.Abstract { Name=the-float | IsNull=true | Constraint=CanBeNull } }",
      "datatype.Date { datatype.Abstract { Name=date | IsNull=false | Constraint=CanNotBeNull } | Value='0 sec' }",
      nullptr
   };

   const std::string str = object->asString();

   int ii = 0;
   while(lines[ii] != nullptr) {
      ASSERT_TRUE(str.find(lines[ii ++]) != std::string::npos);
   }
}
