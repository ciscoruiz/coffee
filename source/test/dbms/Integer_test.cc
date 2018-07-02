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

#include <coffee/dbms/datatype/Integer.hpp>
#include <coffee/dbms/datatype/Float.hpp>

using namespace coffee;
using namespace coffee::dbms;

TEST(IntegerTest,is_nulleable)
{
   datatype::Integer column("nulleable", datatype::Constraint::CanBeNull);

   ASSERT_FALSE(column.hasValue());

   column.clear();

   ASSERT_FALSE(column.hasValue());

   ASSERT_THROW(column.getValue(), basis::RuntimeException);

   column.setValue(10);
   ASSERT_TRUE(column.hasValue());
   ASSERT_EQ(10, column.getValue());

   column.clear();
   ASSERT_FALSE(column.hasValue());
}

TEST(IntegerTest,is_not_nulleable)
{
   datatype::Integer column("not_nulleable", datatype::Constraint::CanNotBeNull);

   ASSERT_TRUE(column.hasValue());

   column.setValue(11);
   ASSERT_TRUE(column.hasValue());
   ASSERT_EQ(11, column.getValue());

   column.clear();
   ASSERT_TRUE(column.hasValue());
}

TEST(IntegerTest,downcast)
{
   datatype::Integer column("integer_downcast");

   datatype::Abstract& abs = column;

   const datatype::Integer& other = coffee_datatype_downcast(datatype::Integer, abs);
   column.setValue(1234);

   ASSERT_TRUE(other == column);

   datatype::Float zzz("zzz");

   ASSERT_THROW(coffee_datatype_downcast(datatype::Integer, zzz), dbms::InvalidDataException);
}

TEST(IntegerTest,clone)
{
   datatype::Integer cannotBeNull("cannotBeNull", datatype::Constraint::CanNotBeNull);
   datatype::Integer canBeNull("canBeNull", datatype::Constraint::CanBeNull);

   ASSERT_TRUE(cannotBeNull.hasValue());
   ASSERT_FALSE(canBeNull.hasValue());

   auto notnull(cannotBeNull.clone());
   auto null(canBeNull.clone());

   ASSERT_TRUE(notnull->hasValue());
   ASSERT_FALSE(null->hasValue());

   ASSERT_EQ(0, notnull->compare(cannotBeNull));

   cannotBeNull.setValue(5.0);

   ASSERT_EQ(5.0, cannotBeNull.getValue());

   notnull = cannotBeNull.clone();
   ASSERT_TRUE(notnull->hasValue());
   ASSERT_EQ(0, notnull->compare(cannotBeNull));

   canBeNull.setValue(25);
   null = canBeNull.clone();
   ASSERT_TRUE(null->hasValue());
   ASSERT_EQ(0, null->compare(canBeNull));

   ASSERT_EQ(20, null->compare(cannotBeNull));

   ASSERT_EQ(-20, notnull->compare(canBeNull));
}

TEST(IntegerTest,instantiate) {
   auto data = datatype::Integer::instantiate("nulleable");
   ASSERT_TRUE(data->hasValue());

   data = datatype::Integer::instantiate("not-nulleable", datatype::Constraint::CanBeNull);
   ASSERT_TRUE(!data->hasValue());
}
