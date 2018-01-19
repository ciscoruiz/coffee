// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2018 Francisco Ruiz Rayo
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
#include <wepa/balance/StrategyRoundRobin.hpp>
#include <wepa/balance/GuardResourceList.hpp>

#include <wepa/logger/Logger.hpp>
#include <wepa/logger/TraceMethod.hpp>

using namespace wepa;

std::shared_ptr<balance::Resource> balance::StrategyRoundRobin::apply()
   throw (ResourceUnavailableException)
{
   GuardResourceList guard(m_resources);
   return apply(guard);
}

std::shared_ptr<balance::Resource> balance::StrategyRoundRobin::apply (GuardResourceList& guard)
   throw (ResourceUnavailableException)
{
   logger::TraceMethod tm (logger::Level::Local7, WEPA_FILE_LOCATION);

   if (!m_position) {
      m_position = m_resources->resource_begin(guard);
   }

   if (m_position.value() == m_resources->resource_end(guard)) {
      WEPA_THROW_NAMED_EXCEPTION(ResourceUnavailableException, m_resources->getName() << " is empty");
   }

   std::shared_ptr<Resource> result;
   ResourceList::resource_iterator ww;
   ResourceList::resource_iterator end;

   ww = end = *m_position;

   do {
      std::shared_ptr<Resource>& w = ResourceList::resource(ww);

      if (w->isAvailable () == true) {
         // prepare the next call to this method
         m_position = m_resources->next(guard, ww);
         result = w;
         break;
      }
   } while ((ww = m_resources->next(guard, ww)) != end);

   if (!result) {
      WEPA_THROW_NAMED_EXCEPTION(ResourceUnavailableException, this->asString() << " there is not any available resource");
   }

   LOG_LOCAL7("Result=" << result->asString());

   return result;
}

