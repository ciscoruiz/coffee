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

#include <coffee/dbms/datatype/LongBlock.hpp>
#include <coffee/dbms/datatype/Integer.hpp>

using namespace coffee;
using namespace coffee::dbms;

TEST(LongBlockTest,is_nulleable)
{
   datatype::LongBlock column("nulleable", datatype::Constraint::CanBeNull);

   ASSERT_FALSE(column.hasValue());

   column.clear();

   ASSERT_FALSE(column.hasValue());

   basis::DataBlock other;
   ASSERT_THROW(column.getValue(), basis::RuntimeException);

   other.assign("hello world", 7);

   column.setValue(other);
   ASSERT_TRUE(column.hasValue());
   ASSERT_EQ(7, column.getSize());
   ASSERT_EQ(0, strncmp(column.getValue().data(), "hello w", 7));

   column.clear();
   ASSERT_FALSE(column.hasValue());
   ASSERT_EQ(0, column.getSize());
}

TEST(LongBlockTest,is_not_nulleable)
{
   datatype::LongBlock column("not_nulleable", datatype::Constraint::CanNotBeNull);

   ASSERT_TRUE(column.hasValue());

   basis::DataBlock other("hello world", 7);

   column.setValue(other);
   ASSERT_TRUE(column.hasValue());
   ASSERT_EQ(7, column.getSize());
   ASSERT_EQ(0, strncmp(column.getValue().data(), "hello w", 7));

   column.clear();
   ASSERT_TRUE(column.hasValue());
   ASSERT_EQ(0, column.getSize());
}

TEST(LongBlockTest,downcast)
{
   datatype::LongBlock column("not_nulleable");

   datatype::Abstract& abs = column;

   auto other = coffee_datatype_downcast(datatype::LongBlock, abs);

   datatype::Integer zzz("zzz");

   ASSERT_THROW(coffee_datatype_downcast(datatype::LongBlock, zzz), dbms::InvalidDataException);
}

TEST(LongBlockTest,clone)
{
   datatype::LongBlock column("not_nulleable");

   const char* buffer = new char[1024];
   basis::DataBlock memory(buffer, 1024);

   column.setValue(memory);

   auto clone = coffee_datatype_downcast(datatype::LongBlock, column.clone());

   ASSERT_TRUE(clone->getValue() == column.getValue());
}

TEST(LongBlockTest,clone_innerscope)
{
   std::shared_ptr<datatype::LongBlock> clone;

   {
      datatype::LongBlock column("not_nulleable");
      basis::DataBlock memory("1234", 4);
      column.setValue(memory);
      clone = coffee_datatype_downcast(datatype::LongBlock, column.clone());
   }

   ASSERT_TRUE(clone->getValue().size() == 4);
   ASSERT_TRUE(memcmp(clone->getValue().data(), "1234", 4) == 0);
}

TEST(LongBlockTest,instantiate) {
   auto data = datatype::LongBlock::instantiate("nulleable");
   ASSERT_TRUE(data->hasValue());

   data = datatype::LongBlock::instantiate("not-nulleable", datatype::Constraint::CanBeNull);
   ASSERT_TRUE(!data->hasValue());
}
