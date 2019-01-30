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

#ifndef __coffee_balance_StrategyByRange_hpp
#define __coffee_balance_StrategyByRange_hpp

#include <vector>
#include <memory>
#include <tuple>

#include "Strategy.hpp"

namespace coffee {
namespace balance {

class GuardResourceContainer;

/**
 * Select a resource by using two level of selections.
 * \li The first step will select the range that contains the strategy to be used in the selection.
 * \li The second step will select the resource by using the strategy selected in the first step.
 *
 * Every range could has a different strategy of selection.
 *
 * \include test/balance/StrategyByRange_test.cc
 */
class StrategyByRange : public Strategy {
   typedef std::tuple<int, int, std::shared_ptr<Strategy> > Range;
   typedef std::vector<Range> Ranges;
   typedef Ranges::iterator range_iterator;

public:
   /**
    * Constructor
    */
   StrategyByRange ();

   /**
    * Destructor
    */
   ~StrategyByRange () { m_ranges.clear (); }

   /**
    * \warning Once you call this method you can not append more resources to this \em balanceIf
    * @param bottom Minimal value for this range
    * @param top Maximal value for this range
    * @param strategy Selection strategy used under this range.
    */
   void addRange (const int bottom, const int top, std::shared_ptr<Strategy> strategy) throw (basis::RuntimeException);

   std::shared_ptr<Resource> apply(const Request& request) throw (ResourceUnavailableException) ;

   std::shared_ptr<xml::Node> asXML(std::shared_ptr<xml::Node>& parent) const throw(basis::RuntimeException);

private:
   Ranges m_ranges;

   range_iterator findRange ( const int identifier) noexcept;
   range_iterator range_begin() noexcept { return m_ranges.begin (); }
   range_iterator range_end() noexcept { return m_ranges.end (); }
   static Range& range(range_iterator ii) noexcept { return *ii; }
};

} /* namespace balance */
} /* namespace coffee */
#endif
