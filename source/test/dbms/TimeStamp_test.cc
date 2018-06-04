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

#include <coffee/dbms/datatype/TimeStamp.hpp>
#include <coffee/dbms/datatype/Integer.hpp>

using namespace coffee;
using namespace coffee::dbms;

TEST(TimeStampTest,is_not_nulleable)
{
   datatype::TimeStamp column("not_nulleable", datatype::Constraint::CanNotBeNull);

   ASSERT_TRUE(column.hasValue());

   ASSERT_THROW(column.isNull(), basis::RuntimeException);

   column.clear();
   ASSERT_TRUE(column.hasValue());
   ASSERT_EQ(std::chrono::seconds::zero(), column.getValue());
}

TEST(TimeStampTest,downcast)
{
   datatype::TimeStamp column("not_nulleable", datatype::Constraint::CanNotBeNull);

   datatype::Abstract& abs(column);

   ASSERT_TRUE(abs.hasValue());

   const datatype::TimeStamp& other = coffee_datatype_downcast(datatype::TimeStamp, abs);

   const char* format = "%d/%m/%YT%H:%M:%S";
   std::string str_TimeStamp("01/01/1920T12:30:50");
   column.setValue(str_TimeStamp, format);

   ASSERT_TRUE(other == column);

   datatype::Integer zzz("zzz");

   ASSERT_THROW(coffee_datatype_downcast(datatype::TimeStamp, zzz), dbms::InvalidDataException);
}

TEST(TimeStampTest,instantiate) {
   auto data = datatype::TimeStamp::instantiate("nulleable");
   ASSERT_TRUE(data->hasValue());

   data = datatype::TimeStamp::instantiate("not-nulleable", datatype::Constraint::CanBeNull);
   ASSERT_TRUE(!data->hasValue());
}
