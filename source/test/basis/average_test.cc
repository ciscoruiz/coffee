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

#include <coffee/basis/Average.hpp>

#include <limits.h>

using namespace coffee;

TEST(AverageTest, basic)
{
   basis::Average <int> average ("basic");

   ASSERT_TRUE(average.isEmpty());
   ASSERT_TRUE(average.isZero ());

   average = 10;
   average = 5;
   average += 7;

   ASSERT_EQ(6, average.value ());
   ASSERT_EQ(6, average);

   ASSERT_FALSE(average.isEmpty());
   ASSERT_FALSE(average.isZero ());
   ASSERT_EQ(2, average.size ());

   ASSERT_EQ(12, average.getAccumulator());

   average.clear ();
   ASSERT_TRUE(average.isEmpty());
   ASSERT_TRUE(average.isZero ());
   ASSERT_EQ(0, average.value ());
   ASSERT_EQ(0, average);
}

TEST(AverageTest, signed_overflow )
{
   basis::Average <int> average ("integer");

   const int someValue = 157234;

   int nloop = INT_MAX / someValue;

   for (int ii = 0; ii < nloop; ++ ii) {
      average += someValue;
   }

   ASSERT_EQ(nloop, average.size ());
   ASSERT_EQ(someValue, average.value ());

   int overflow = INT_MAX - average.getAccumulator();

   ASSERT_GT(overflow, 0);

   average += (overflow + 1);

   ASSERT_EQ(1, average.size ());
   ASSERT_EQ(overflow + 1, average.value ());

}

TEST(AverageTest, unsigned_overflow)
{
   basis::Average <unsigned int> average ("unsigned");

   const unsigned int someValue = 157234;

   int nloop = UINT_MAX / someValue;

   for (int ii = 0; ii < nloop; ++ ii) {
      average += someValue;
   }

   ASSERT_EQ(nloop, average.size ());
   ASSERT_EQ(someValue, average.value ());

   unsigned int overflow = UINT_MAX - average.getAccumulator();

   ASSERT_GT(overflow, 0);

   average += (overflow + 1);

   ASSERT_EQ(1, average.size ());
   ASSERT_EQ(overflow + 1, average.value ());
}
