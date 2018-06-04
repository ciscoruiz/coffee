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

#include <coffee/dbms/datatype/Integer.hpp>

using namespace coffee;

TEST(ClassBuilderTest, twice_pks)
{
   std::shared_ptr<persistence::PrimaryKey> primaryKey;

   {
      persistence::PrimaryKeyBuilder pkBuilder;
      pkBuilder.add(std::make_shared<dbms::datatype::Integer>("id"));
      primaryKey = pkBuilder.build();
   }

   persistence::ClassBuilder classBuilder("customer");
   ASSERT_NO_THROW(classBuilder.set(primaryKey));

   std::shared_ptr<persistence::PrimaryKey> badPrimaryKey;
   {
      persistence::PrimaryKeyBuilder pkBuilder;
      pkBuilder.add(std::make_shared<dbms::datatype::Integer>("some-other-stuff"));
      badPrimaryKey = pkBuilder.build();
   }

   ASSERT_THROW(classBuilder.set(badPrimaryKey), basis::RuntimeException);
}

TEST(ClassBuilderTest, without_pk)
{
   persistence::ClassBuilder classBuilder("customer");

   classBuilder.add(std::make_shared<dbms::datatype::Integer>("id")).add(std::make_shared<dbms::datatype::Integer>("age"));
   ASSERT_THROW(classBuilder.build(), basis::RuntimeException);
}

TEST(ClassBuilderTest, without_fields)
{
   persistence::ClassBuilder classBuilder("customer");

   persistence::PrimaryKeyBuilder pkBuilder;
   pkBuilder.add(std::make_shared<dbms::datatype::Integer>("id"));
   auto primaryKey = pkBuilder.build();

   ASSERT_NO_THROW(classBuilder.set(primaryKey));
   ASSERT_THROW(classBuilder.build(), basis::RuntimeException);
}

TEST(ClassBuilderTest, dual_member)
{
   static const char* NameInPkAndMember = "id";
   persistence::ClassBuilder classBuilder("customer");

   persistence::PrimaryKeyBuilder pkBuilder;
   pkBuilder.add(std::make_shared<dbms::datatype::Integer>(NameInPkAndMember));
   auto primaryKey = pkBuilder.build();

   ASSERT_NO_THROW(classBuilder.set(primaryKey));
   classBuilder.add(std::make_shared<dbms::datatype::Integer>("size")).add(std::make_shared<dbms::datatype::Integer>(NameInPkAndMember));

   ASSERT_THROW(classBuilder.build(), basis::RuntimeException);
}
