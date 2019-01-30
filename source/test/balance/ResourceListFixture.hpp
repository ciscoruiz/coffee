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
#ifndef TEST_BALANCE_RESOURCELISTFIXTURE_HPP_
#define TEST_BALANCE_RESOURCELISTFIXTURE_HPP_

#include <coffee/balance/ResourceContainer.hpp>

#include "TestResource.hpp"

struct ResourceListFixture : public ::testing::Test {
   static const int MaxResources;

   ResourceListFixture()
   {

      resourceList = setup(MaxResources, 0);
   }

   static std::shared_ptr<coffee::balance::ResourceContainer> setup(const int maxResources, const int firstId) {
      auto result = std::make_shared<coffee::balance::ResourceContainer>("TestResources");

      int id = firstId;
      for (int ii = 0; ii < maxResources; ++ ii) {
         result->add(std::make_shared<coffee::test::balance::TestResource>(id ++));
      }

      result->initialize();
      return result;
   }

   std::shared_ptr<coffee::balance::ResourceContainer> resourceList;
};

#endif /* TEST_BALANCE_RESOURCELISTFIXTURE_HPP_ */
