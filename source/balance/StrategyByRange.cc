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

#include <coffee/balance/Resource.hpp>
#include <coffee/balance/ResourceList.hpp>
#include <coffee/balance/StrategyByRange.hpp>
#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TraceMethod.hpp>
#include <coffee/balance/GuardResourceList.hpp>

using namespace coffee;

balance::StrategyByRange::StrategyByRange() :
   balance::Strategy("balance::ByRange"),
   m_unusedList(std::make_shared<ResourceList>("balance::ByRange::unused")),
   m_key(0)
{
   setResourceList(m_unusedList);
}

void balance::StrategyByRange::addRange (const int bottom, const int top, std::shared_ptr<Strategy> strategy)
   throw (adt::RuntimeException)
{
   GuardResourceList guard(m_unusedList);

   if (!strategy)
      COFFEE_THROW_EXCEPTION(asString () << " can not associate a null Strategy");

   if (strategy.get() == this)
      COFFEE_THROW_EXCEPTION(asString () << " can not generate a recursive association");

   if (bottom > top)
      COFFEE_THROW_EXCEPTION(asString () << " | invalid range (bottom > top)");

   for (range_iterator ii = range_begin(), maxii = range_end(); ii != maxii; ++ ii) {
      Range& range = StrategyByRange::range(ii);
      if (std::get<0>(range) >= bottom && std::get<1>(range) <= top) {
         COFFEE_THROW_EXCEPTION(std::get<2>(range)->asString () << " would be hidden by " << strategy->asString());
      }
   }

   auto range = std::make_tuple(top, bottom, strategy);

   LOG_DEBUG ("Range (" <<  bottom << "," << top << "): " << strategy->asString () << " has been created");

   m_ranges.push_back(range);
}

balance::StrategyByRange::range_iterator balance::StrategyByRange::findRange (GuardResourceList&, const int key)
   noexcept
{
   static std::shared_ptr<balance::Strategy> empty;

   for (range_iterator ii = range_begin(), maxii = range_end(); ii != maxii; ++ ii) {
      Range& range = StrategyByRange::range(ii);
      if (std::get<0>(range) >= key && std::get<1>(range) <= key) {
         return ii;
      }
   }

   return range_end();
}

std::shared_ptr<balance::Resource> balance::StrategyByRange::apply(const int key)
   throw (ResourceUnavailableException)
{
   GuardResourceList guard(m_unusedList);
   m_key = key;
   return apply(guard);
}

std::shared_ptr<balance::Resource> balance::StrategyByRange::apply(GuardResourceList& guard)
   throw (balance::ResourceUnavailableException)
{
   logger::TraceMethod tm (logger::Level::Local7, COFFEE_FILE_LOCATION);

   std::shared_ptr<balance::Strategy> strategy;

   if (true) {
      range_iterator ii = findRange (guard, m_key);

      if (ii == range_end()) {
         COFFEE_THROW_NAMED_EXCEPTION(balance::ResourceUnavailableException, "Key=" << m_key << " does not have a defined range");
      }

      strategy = std::get<2>(range(ii));
   }

   GuardResourceList guard2(strategy->getResourceList());

   return strategy->apply(guard2);
}


