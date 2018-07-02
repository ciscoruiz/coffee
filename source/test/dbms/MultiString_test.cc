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

#include <coffee/dbms/datatype/MultiString.hpp>
#include <coffee/dbms/datatype/Integer.hpp>

using namespace coffee::dbms;

struct MultiStringTestFixture : ::testing::Test {
   static const char* values[];

   MultiStringTestFixture() : column("the-column", datatype::Constraint::CanNotBeNull) {;}

   void SetUp() {
      int index = 0;
      while (values[index]) {
         column.addValue(values[index ++]);
      }
      ASSERT_TRUE(column.size() == index);
   }

   datatype::MultiString column;
};

// static
const char* MultiStringTestFixture::values[] = { "1234", "3456", "1234", nullptr };

TEST(MultiStringTest,is_nulleable)
{
   datatype::MultiString column("nulleable", datatype::Constraint::CanBeNull);
   ASSERT_TRUE(!column.hasValue());
   column.addValue("the value");
   ASSERT_TRUE(column.hasValue());
}

TEST_F(MultiStringTestFixture, iterator)
{
   ASSERT_TRUE(column.hasValue());

   int index = 0;
   for (auto ii = column.begin_value(), maxii = column.end_value(); ii != maxii; ++ ii) {
      ASSERT_EQ(values[index ++], datatype::MultiString::value(ii));
   }
}

TEST_F(MultiStringTestFixture, clone)
{
   ASSERT_TRUE(column.hasValue());

   auto clone = column.clone();

   ASSERT_TRUE(clone->compare(column) == 0);
   ASSERT_TRUE(clone->hash() == column.hash());
}

TEST_F(MultiStringTestFixture, clear)
{
   ASSERT_TRUE(column.hasValue());
   column.clear();
   ASSERT_TRUE(column.hasValue());
   ASSERT_EQ(0, column.size());
}

TEST(MultiStringTest, downcast)
{
   datatype::MultiString column("multistring_downcast");

   datatype::Abstract& abs = column;

   const datatype::MultiString& other = coffee_datatype_downcast(datatype::MultiString, abs);
   column.addValue("123");

   ASSERT_TRUE(other == column);

   datatype::Integer zzz("zzz");

   ASSERT_THROW(coffee_datatype_downcast(datatype::MultiString, zzz), InvalidDataException);
}

TEST(MultiStringTest, instantiate) {
   auto data = datatype::MultiString::instantiate("nulleable");
   ASSERT_TRUE(data->hasValue());

   data = datatype::MultiString::instantiate("not-nulleable", datatype::Constraint::CanBeNull);
   ASSERT_TRUE(!data->hasValue());
}
