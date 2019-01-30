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

#ifndef __coffee_balance_StrategyRoundRobin_hpp
#define __coffee_balance_StrategyRoundRobin_hpp

#include "Strategy.hpp"
#include "ResourceContainer.hpp"

namespace coffee {
namespace balance {

class GuardResourceContainer;

/**
 * Select a resource by using a Round Robin technique. If the candidate resource would not be available
 * then it will start a sequential search for the first available resource.
 *
 * \include test/balance/StrategyRoundRobin_test.cc
 *
 */
class StrategyRoundRobin : public Strategy {
public:
   /**
    * Constuctor.
    * \param resources List of resources to work with.
    */
   explicit StrategyRoundRobin (std::shared_ptr<ResourceContainer>& resources) : Strategy("balance::RoundRobin", resources) {;}

   /**
    * Select a resource by using a Round Robin technique. If the candidate resource would not be available
    * will start a sequential search for the first available resource.
    * \return the selected resource.
    */
    std::shared_ptr<Resource> apply(const Request& request) throw (ResourceUnavailableException) ;

   std::shared_ptr<xml::Node> asXML(std::shared_ptr<xml::Node>& parent) const throw(basis::RuntimeException);

private:
   std::pair<bool, ResourceContainer::resource_iterator> m_position;
};

} /* namespace balance */
} /* namespace coffee */
#endif
