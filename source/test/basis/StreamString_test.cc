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

#include <limits.h>

#include <iostream>

#include <gtest/gtest.h>

#include <coffee/basis/StreamString.hpp>

using namespace std;
using namespace coffee;

TEST(StreamStringTest, integer )
{
   int val = 10;

   basis::StreamString result;

   result = val;
   ASSERT_EQ("10",result);

   result = 0;
   ASSERT_EQ("0",result);

   val = INT_MAX;
   result = val;
   ASSERT_EQ("2147483647",result);

   val = INT_MIN;
   result = val;
   ASSERT_EQ("-2147483648",result);
}

TEST(StreamStringTest, uinteger )
{
   unsigned int val = 10;

   basis::StreamString result;

   result = val;
   ASSERT_EQ("10",result);

   result = 0;
   ASSERT_EQ("0",result);

   val = UINT_MAX;
   result = val;
   ASSERT_EQ("4294967295",result);
}

TEST(StreamStringTest, integer64 )
{
   int64_t val = 0;
   basis::StreamString result;

   val --;
   result = val;
   ASSERT_EQ("-1",result);

   val = INT64_MAX;
   result = val;
   ASSERT_EQ("9223372036854775807",result);

   val = INT64_MIN;
   result = val;
   ASSERT_EQ("-9223372036854775808",result);
}

TEST(StreamStringTest, uinteger64 )
{
   uint64_t val = 0;
   basis::StreamString result;

   result = val;
   ASSERT_EQ("0",result);

   val = UINT64_MAX;
   result = val;
   ASSERT_EQ("18446744073709551615",result);
}

TEST(StreamStringTest, bool )
{
   basis::StreamString result;

   result = true;
   ASSERT_EQ("true",result);

   result = false;
   ASSERT_EQ("false",result);
}

TEST(StreamStringTest, char_char )
{
   basis::StreamString result;

   result << "hello" << " " << "world";

   ASSERT_EQ("hello world",result);
}

TEST(StreamStringTest, char_int )
{
   basis::StreamString result;

   result << "hello" << " " << 3 << "rd world";

   ASSERT_EQ("hello 3rd world",result);
}

TEST(StreamStringTest, string_bool )
{
   basis::StreamString result;

   std::string hello ("hello");
   result << hello << " " << true << " world";

   ASSERT_EQ("hello true world",result);
}

TEST(StreamStringTest, string_int64 )
{
   basis::StreamString result;

   result << "max_int64:" << INT64_MAX;
   result.toUpper();
   ASSERT_EQ("MAX_INT64:9223372036854775807",result);
}

TEST(StreamStringTest, string_null )
{
   basis::StreamString result(basis::StreamString::Flag::ShowNull);

   result << "text=" << (const char*) NULL;
   ASSERT_EQ("text=<null>",result);
}

TEST(StreamStringTest, string_double )
{
   basis::StreamString result;

   result << "double=" << (double) 0.000123;
   ASSERT_EQ("double=1.230000e-04",result);
}

TEST(StreamStringTest, chrono )
{
   std::chrono::seconds sec (100);
   std::chrono::milliseconds ms(sec);
   std::chrono::microseconds us(sec);

   basis::StreamString result;

   result = sec;
   result << " | " << ms;
   result << " | " << us;
   ASSERT_EQ("100 sec | 100000 ms | 100000000 us",result);
}
