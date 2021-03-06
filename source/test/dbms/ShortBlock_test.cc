// MIT License
// 
// Copyright(c) 2018 Francisco Ruiz(francisco.ruiz.rayo@gmail.com)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
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

#include <coffee/dbms/datatype/ShortBlock.hpp>
#include <coffee/dbms/datatype/Integer.hpp>

using namespace coffee;
using namespace coffee::dbms;

TEST(ShortBlockTest,is_nulleable)
{
   datatype::ShortBlock column("nulleable", 128, datatype::Constraint::CanBeNull);

   void* init = column.getBuffer();

   ASSERT_FALSE(column.hasValue());

   column.clear();

   ASSERT_FALSE(column.hasValue());

   basis::DataBlock other;
   ASSERT_THROW(column.getValue(), basis::RuntimeException);

   other.assign("hello world", 7);

   ASSERT_EQ(7, other.size());

   column.setValue(other);
   ASSERT_TRUE(column.hasValue());
   ASSERT_EQ(7, column.getSize());
   ASSERT_EQ(0, strncmp(column.getValue().data(), "hello w", 7));

   column.clear();
   ASSERT_FALSE(column.hasValue());
   ASSERT_EQ(0, column.getSize());

   ASSERT_EQ(column.getBuffer(), init);
}

TEST(ShortBlockTest,is_not_nulleable)
{
   datatype::ShortBlock column("not_nulleable", 4, datatype::Constraint::CanNotBeNull);

   void* init = column.getBuffer();

   ASSERT_TRUE(column.hasValue());

   basis::DataBlock other("hello world", 7);

   ASSERT_THROW(column.setValue(other), basis::RuntimeException);

   other.assign("hello", 4);
   column.setValue(other);
   ASSERT_TRUE(column.hasValue());
   ASSERT_EQ(4, column.getSize());
   ASSERT_EQ(0, strncmp(column.getValue().data(), "hello w", 4));

   column.clear();
   ASSERT_TRUE(column.hasValue());
   ASSERT_EQ(0, column.getSize());

   ASSERT_EQ(column.getBuffer(), init);

   other.assign("size out of range");
   ASSERT_THROW(column.setValue(other), basis::RuntimeException);
}

TEST(ShortBlockTest,downcast)
{
   datatype::ShortBlock column("not_nulleable", 4);

   datatype::Abstract& abs = column;

   auto other = coffee_datatype_downcast(datatype::ShortBlock, abs);

   datatype::Integer zzz("zzz");

   ASSERT_THROW(coffee_datatype_downcast(datatype::ShortBlock, zzz), dbms::InvalidDataException);
}

TEST(ShortBlockTest,clone)
{
   datatype::ShortBlock column("not_nulleable", 1025);

   const char* buffer = new char[1024];
   basis::DataBlock memory(buffer, 1024);

   column.setValue(memory);

   auto clone = coffee_datatype_downcast(datatype::ShortBlock, column.clone());

   ASSERT_TRUE(clone->getValue() == column.getValue());
}

TEST(ShortBlockTest,instantiate) 
{
   auto data = datatype::ShortBlock::instantiate("nulleable", 10);
   ASSERT_TRUE(data->hasValue());
   ASSERT_EQ(10, data->getMaxSize());

   data = datatype::ShortBlock::instantiate("not-nulleable", 20, datatype::Constraint::CanBeNull);
   ASSERT_FALSE(data->hasValue());
   ASSERT_EQ(20, data->getMaxSize());
}
