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

#include <limits.h>
#include <stdio.h>

#include <iostream>

#include <gtest/gtest.h>

#include <coffee/basis/AsString.hpp>
#include <coffee/basis/DataBlock.hpp>

using namespace std;
using namespace coffee;

TEST(AsStringTest, integer)
{
   int val = 10;

   ASSERT_EQ("10", basis::AsString::apply(val));
   ASSERT_EQ("00010", basis::AsString::apply(val, "%05d"));
   ASSERT_EQ("0", basis::AsString::apply(0));

   val = INT_MAX;
   ASSERT_EQ("2147483647", basis::AsString::apply(val));

   val = INT_MIN;
   ASSERT_EQ("-2147483648", basis::AsString::apply(val));
}

TEST(AsStringTest, uinteger)
{
   unsigned int val = 10;

   ASSERT_EQ("10", basis::AsString::apply(val));
   ASSERT_EQ("0", basis::AsString::apply(0));

   val = UINT_MAX;
   ASSERT_EQ("4294967295", basis::AsString::apply(val));
}

TEST(AsStringTest, integer64)
{
   int64_t val = 0;

   val --;
   ASSERT_EQ("-1", basis::AsString::apply(val));

   val = INT64_MAX;
   ASSERT_EQ("9223372036854775807", basis::AsString::apply(val));

   val = INT64_MIN;
   ASSERT_EQ("-9223372036854775808", basis::AsString::apply(val));
}

TEST(AsStringTest, uinteger64)
{
   uint64_t val = 0;

   ASSERT_EQ("0", basis::AsString::apply(val));

   val = UINT64_MAX;
   ASSERT_EQ("18446744073709551615", basis::AsString::apply(val));
}

TEST(AsStringTest, bool)
{
   ASSERT_EQ("true", basis::AsString::apply(true));
   ASSERT_EQ("false", basis::AsString::apply(false));
}

TEST(AsStringTest, datablock)
{
   char buffer[1024];
   basis::DataBlock value(buffer, 1024);

   const string result = basis::AsString::apply(value);

   char number[5];
   for(int ii = 0; ii <=(1024 - basis::AsString::DefaultCharactersByLine); ii += basis::AsString::DefaultCharactersByLine) {
      sprintf(number, "%4d: ", ii);
      ASSERT_TRUE(result.find(number) != std::string::npos) << "Number=" << ii;
   }

   ASSERT_GT(result.size(), 0);

   value.clear();
   ASSERT_EQ("<null>", basis::AsString::apply(value));
}

TEST(AsStringTest, double)
{
   ASSERT_EQ("123.13", basis::AsString::apply(123.127, "%.02f"));
}
