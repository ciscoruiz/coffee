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

#include <iostream>

#include <limits.h>

#include <coffee/config/Release.hpp>
#include <coffee/config/defines.hpp>

using namespace std;
using namespace coffee;

TEST(ConfigTest, release)
{
   string version = config::Release::getVersion();

   cout << version << endl;

   ASSERT_FALSE (version.empty());

   int debug = version.find("/D");
   int release = version.find("/O");

   #ifdef _DEBUG
      ASSERT_TRUE (debug != string::npos);
      ASSERT_TRUE (release == string::npos);
   #else
      ASSERT_TRUE (debug == string::npos);
      ASSERT_TRUE (release != string::npos);
   #endif
}

TEST(ConfigTest, numbers)
{
   int64_t ii64;

   ii64 = LLONG_MAX;
   ASSERT_EQ (ii64, LLONG_MAX);

   ii64 = LLONG_MIN;
   ASSERT_EQ (ii64, LLONG_MIN);

   uint64_t u64;
   u64 = ULLONG_MAX;
   ASSERT_EQ (u64, ULLONG_MAX);
}

int main(int argc, char **argv) {
   testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
