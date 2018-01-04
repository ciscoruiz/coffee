// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2013 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/wepa
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#include <wepa/balance/Resource.hpp>
#include <wepa/balance/ResourceList.hpp>
#include <wepa/balance/StrategyByRange.hpp>
#include <wepa/logger/Logger.hpp>
#include <wepa/logger/TraceMethod.hpp>

using namespace wepa;

balance::StrategyByRange::StrategyByRange() :
   balance::Strategy("balance::ByRange"),
   m_unusedList(std::make_shared<ResourceList>("balance::ByRange::unused")),
   m_key(0)
{
   setResourceList(m_unusedList);
}

void balance::StrategyByRange::addRange (const int bottom, const int top, std::shared_ptr<Strategy>& strategy)
   throw (adt::RuntimeException)
{
   ResourceList::LockGuard guard(m_unusedList);

   if (!strategy)
      WEPA_THROW_EXCEPTION(asString () << " can not associate a null Strategy");

   if (strategy.get() == this)
      WEPA_THROW_EXCEPTION(asString () << " can not generate a recursive association");

   if (bottom > top)
      WEPA_THROW_EXCEPTION(asString () << " | invalid range (bottom > top)");

   for (range_iterator ii = range_begin(), maxii = range_end(); ii != maxii; ++ ii) {
      Range& range = StrategyByRange::range(ii);
      if (std::get<0>(range) >= bottom && std::get<1>(range) <= top) {
         WEPA_THROW_EXCEPTION(std::get<2>(range)->asString () << " would be hidden by " << strategy->asString());
      }
   }

   range_iterator ii;

   if ((ii = findRange(guard, bottom)) != range_end()) {
      WEPA_THROW_EXCEPTION(std::get<2>(range(ii))->asString () << " has overlapping with " << strategy->asString());
   }

   if ((ii = findRange(guard, top)) != range_end()) {
      WEPA_THROW_EXCEPTION(std::get<2>(range(ii))->asString () << " has overlapping with " << strategy->asString());
   }

   auto range = std::make_tuple(top, bottom, strategy);

   LOG_DEBUG ("Range (" <<  bottom << "," << top << "): " << strategy->asString () << " has been created");

   m_ranges.push_back(range);
}

balance::StrategyByRange::range_iterator balance::StrategyByRange::findRange (ResourceList::LockGuard&, const int key)
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

std::shared_ptr<balance::Resource> balance::StrategyByRange::apply(ResourceList::LockGuard& guard)
   throw (balance::ResourceUnavailableException)
{
   logger::TraceMethod tm (logger::Level::Local7, WEPA_FILE_LOCATION);

   std::shared_ptr<balance::Strategy> strategy;

   if (true) {
      range_iterator ii = findRange (guard, m_key);

      if (ii == range_end()) {
         WEPA_THROW_NAMED_EXCEPTION(balance::ResourceUnavailableException, "Key=" << m_key << " does not have a defined range");
      }

      strategy = std::get<2>(range(ii));
   }

   ResourceList::LockGuard guard2(strategy->getResourceList());

   return strategy->apply(guard2);
}


