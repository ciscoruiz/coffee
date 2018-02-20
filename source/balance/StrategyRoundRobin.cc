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
#include <coffee/balance/StrategyRoundRobin.hpp>
#include <coffee/balance/GuardResourceList.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TraceMethod.hpp>

using namespace coffee;

std::shared_ptr<balance::Resource> balance::StrategyRoundRobin::apply()
   throw (ResourceUnavailableException)
{
   GuardResourceList guard(m_resources);
   return apply(guard);
}

std::shared_ptr<balance::Resource> balance::StrategyRoundRobin::apply (GuardResourceList& guard)
   throw (ResourceUnavailableException)
{
   logger::TraceMethod tm (logger::Level::Local7, COFFEE_FILE_LOCATION);

   if (!m_position) {
      m_position = m_resources->resource_begin(guard);
   }

   if (m_position.value() == m_resources->resource_end(guard)) {
      COFFEE_THROW_NAMED_EXCEPTION(ResourceUnavailableException, m_resources->getName() << " is empty");
   }

   std::shared_ptr<Resource> result;
   ResourceList::resource_iterator ww;
   ResourceList::resource_iterator end;

   ww = end = m_position.value();

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
      COFFEE_THROW_NAMED_EXCEPTION(ResourceUnavailableException, this->asString() << " there is not any available resource");
   }

   LOG_LOCAL7("Result=" << result->asString());

   return result;
}

