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

#include <gtest/gtest.h>

#include <coffee/basis/DataBlock.hpp>
#include <coffee/basis/RuntimeException.hpp>

using namespace std;
using namespace coffee;

TEST(DataBlockTest, size )
{
   basis::DataBlock var("hello xxx", 5);

   ASSERT_EQ(5, var.size());

   var.assign("second");

   ASSERT_EQ(6, var.size());

   var.assign("three", 2);
   ASSERT_EQ(2, var.size());
}

TEST(DataBlockTest, copy )
{
   const char* pp = "hello xxx";

   basis::DataBlock var(pp, strlen(pp) + 1);

   ASSERT_EQ(strlen(pp) + 1, var.size());

   basis::DataBlock other(var);

   ASSERT_EQ(0, strcmp(other.data(), pp));

   basis::DataBlock copy;
   copy = var;
   ASSERT_TRUE(copy == var);
}

TEST(DataBlockTest, buffer ) {
   char buffer[128];

   for (int ii = 0; ii < sizeof(buffer); ++ii)
      buffer[ii] = (char) ii;

   basis::DataBlock var(buffer, sizeof(buffer));

   ASSERT_EQ(sizeof(buffer), var.size());

   for (int ii = 0; ii < sizeof(buffer); ++ii) {
      ASSERT_EQ(var[ii], buffer[ii]) << " Fault on Index=" << ii;
   }
}

TEST(DataBlockTest, append )
{
   char buffer [128];
   basis::DataBlock var;

   for(int ii = 0; ii < sizeof(buffer); ++ ii) {
      buffer [ii] =(char) ii;
      var.append((char) ii);
   }

   ASSERT_EQ(sizeof(buffer), var.size());

   for(int ii = 0; ii < sizeof(buffer); ++ ii) {
      ASSERT_EQ(var [ii], buffer [ii]);
   }
}

TEST(DataBlockTest, clear )
{
   basis::DataBlock var("wow", 3);

   ASSERT_EQ(3, var.size());

   var.clear();
   ASSERT_EQ(0, var.size());
   ASSERT_TRUE(var.empty());
}

TEST(DataBlockTest, operator_parenthesis )
{
   basis::DataBlock var("wow", 3);

   var [1] = 'i';

   const basis::DataBlock copy(var);

   ASSERT_EQ(0, strcmp(var.data(), "wiw"));
   ASSERT_EQ('i', copy [1]);
   ASSERT_EQ('i', var [1]);
}

TEST(DataBlockTest, out_of_range )
{
   basis::DataBlock var("wow", 3);

   char zz;
   ASSERT_THROW(zz = var [10], basis::RuntimeException);
   ASSERT_THROW(var [10] = 0, basis::RuntimeException);

   const basis::DataBlock copy(var);
   ASSERT_THROW(copy [10] == 0, basis::RuntimeException);
   ASSERT_THROW(copy.at(10), basis::RuntimeException);
}
