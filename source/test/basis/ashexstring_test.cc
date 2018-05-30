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

#include <iostream>

#include <gtest/gtest.h>

#include <coffee/basis/AsHexString.hpp>
#include <coffee/basis/DataBlock.hpp>

using namespace std;
using namespace coffee;

TEST(AsHexStringTest,integer)
{
   int val = 10;

   ASSERT_EQ("0xa", basis::AsHexString::apply(val));
   ASSERT_EQ("0x0", basis::AsHexString::apply(0));

   val = INT_MAX;
   ASSERT_EQ("0x7fffffff", basis::AsHexString::apply(val));

   val = INT_MIN;
   ASSERT_EQ("0x80000000", basis::AsHexString::apply(val));
}

TEST(AsHexStringTest,uinteger)
{
   unsigned int val = 10;

   ASSERT_EQ("0xa", basis::AsHexString::apply(val));
   ASSERT_EQ("0x0", basis::AsHexString::apply(0));

   val = UINT_MAX;
   ASSERT_EQ("0xffffffff", basis::AsHexString::apply(val));
}

TEST(AsHexStringTest,integer64)
{
   int64_t val = 0;
   string result;

   val --;
   ASSERT_EQ("0xffffffffffffffff", basis::AsHexString::apply(val));

   val = LLONG_MAX;
   ASSERT_EQ("0x7fffffffffffffff", basis::AsHexString::apply(val));

   val = LLONG_MIN;
   ASSERT_EQ("0x8000000000000000", basis::AsHexString::apply(val));
}

TEST(AsHexStringTest,uinteger64)
{
   uint64_t val = 0;
   string result;

   ASSERT_EQ("0x0", basis::AsHexString::apply(val));

   val = ULLONG_MAX;
   ASSERT_EQ("0xffffffffffffffff", basis::AsHexString::apply(val));
}
