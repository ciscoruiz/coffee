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

#ifndef __coffee_balance_StrategyIndexed_hpp
#define __coffee_balance_StrategyIndexed_hpp

#include "Strategy.hpp"

namespace coffee {
namespace balance {

class GuardResourceList;

class StrategyIndexed : public Strategy {
public:
   explicit StrategyIndexed (std::shared_ptr<ResourceList>& resources) :
      balance::Strategy("balance::Indexed", resources),
      m_key(0)
   {
   }

   std::shared_ptr<Resource> apply(const int key) throw (ResourceUnavailableException);

private:
   int m_key;

   std::shared_ptr<Resource> apply(GuardResourceList& guard) throw (ResourceUnavailableException);
};

} /* namespace balance */
} /* namespace coffee */
#endif
