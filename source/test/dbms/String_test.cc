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

#include <iostream>
#include <time.h>

#include <gtest/gtest.h>

#include <coffee/dbms/datatype/String.hpp>
#include <coffee/dbms/datatype/Integer.hpp>

using namespace coffee;
using namespace coffee::dbms;

TEST(StringTest,is_nulleable)
{
   datatype::String column("nulleable", 16, datatype::Constraint::CanBeNull);

   void* init = column.getBuffer();

   ASSERT_FALSE(column.hasValue());

   column.clear();

   ASSERT_FALSE(column.hasValue());
   ASSERT_THROW(column.getValue(), basis::RuntimeException);
   ASSERT_EQ(0, column.getSize());

   column.setValue("hello world");
   ASSERT_TRUE(column.hasValue());
   ASSERT_EQ(coffee_strlen("hello world"), column.getSize());
   ASSERT_EQ(0, strncmp(column.getValue(), "hello world", coffee_strlen("hello world")));

   column.clear();
   ASSERT_FALSE(column.hasValue());
   ASSERT_EQ(0, column.getSize());

   column.setValue("zzz");
   ASSERT_TRUE(column.hasValue());

   column.setValue(NULL);
   ASSERT_FALSE(column.hasValue());

   ASSERT_EQ(column.getBuffer(), init);

   ASSERT_THROW(column.setValue("size out of range"), basis::RuntimeException);
}

TEST(StringTest,nulleable_asstring)
{
   datatype::String column("nulleable", 16, datatype::Constraint::CanBeNull);
   ASSERT_FALSE(column.hasValue());
   ASSERT_EQ("datatype.String { datatype.Abstract { Name=nulleable | IsNull=true | Constraint=CanBeNull } | MaxSize=16 }", column.asString());
}

TEST(StringTest,is_not_nulleable)
{
   datatype::String column("not_nulleable", 4, datatype::Constraint::CanNotBeNull);

   void* init = column.getBuffer();

   ASSERT_TRUE(column.hasValue());

   column.setValue("bye");
   ASSERT_TRUE(column.hasValue());
   ASSERT_EQ(coffee_strlen("bye"), column.getSize());
   ASSERT_EQ(0, strncmp(column.getValue(), "bye", coffee_strlen("bye")));

   column.clear();
   ASSERT_TRUE(column.hasValue());
   ASSERT_EQ(0, column.getSize());

   ASSERT_EQ(column.getBuffer(), init);

   ASSERT_THROW(column.setValue("size out of range"), basis::RuntimeException);
   ASSERT_THROW(column.setValue(NULL), basis::RuntimeException);
}

TEST(StringTest,downcast)
{
   datatype::String column("string_downcast", 4);

   datatype::Abstract& abs = column;

   const datatype::String& other = coffee_datatype_downcast(datatype::String, abs);
   column.setValue("123");

   ASSERT_TRUE(other == column);

   datatype::Integer zzz("zzz");

   ASSERT_THROW(coffee_datatype_downcast(datatype::String, zzz), dbms::InvalidDataException);
}

TEST(StringTest,clone)
{
   datatype::String cannotBeNull("cannotBeNull", 16, datatype::Constraint::CanNotBeNull);
   datatype::String canBeNull("canBeNull", 16, datatype::Constraint::CanBeNull);

   ASSERT_TRUE(cannotBeNull.hasValue());
   ASSERT_FALSE(canBeNull.hasValue());

   auto notnull(cannotBeNull.clone());
   auto null(canBeNull.clone());

   ASSERT_TRUE(notnull->hasValue());
   ASSERT_FALSE(null->hasValue());

   ASSERT_EQ(0, notnull->compare(cannotBeNull));

   cannotBeNull.setValue("abcd");

   ASSERT_STREQ("abcd", cannotBeNull.getValue());

   notnull = cannotBeNull.clone();
   ASSERT_TRUE(notnull->hasValue());
   ASSERT_EQ(0, notnull->compare(cannotBeNull));

   canBeNull.setValue("xzy");
   null = canBeNull.clone();
   ASSERT_TRUE(null->hasValue());
   ASSERT_EQ(0, null->compare(canBeNull));
   ASSERT_GT(null->compare(cannotBeNull), 0);

   ASSERT_LT(notnull->compare(canBeNull), 0);
}

TEST(StringTest,instantiate) {
   auto data = datatype::String::instantiate("nulleable", 10);
   ASSERT_TRUE(data->hasValue());
   ASSERT_EQ(10, data->getMaxSize());

   data = datatype::String::instantiate("not-nulleable", 20, datatype::Constraint::CanBeNull);
   ASSERT_TRUE(!data->hasValue());
   ASSERT_EQ(20, data->getMaxSize());
}
