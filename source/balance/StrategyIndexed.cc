#include <mutex>

#include <coffee/balance/Resource.hpp>
#include <coffee/balance/ResourceList.hpp>
#include <coffee/balance/StrategyIndexed.hpp>
#include <coffee/balance/GuardResourceList.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TraceMethod.hpp>

using namespace coffee;

std::shared_ptr<balance::Resource> balance::StrategyIndexed::apply(const int key)
   throw (ResourceUnavailableException)
{
   GuardResourceList guard(m_resources);
   m_key = key;
   return apply(guard);
}

std::shared_ptr<balance::Resource> balance::StrategyIndexed::apply(GuardResourceList& guard)
   throw (ResourceUnavailableException)
{
   logger::TraceMethod tm (logger::Level::Local7, COFFEE_FILE_LOCATION);

   if (m_resources->size(guard) == 0) {
      COFFEE_THROW_NAMED_EXCEPTION(ResourceUnavailableException, m_resources->getName() << " is empty");
   }

   std::shared_ptr<Resource> result;
   ResourceList::resource_iterator ww;
   ResourceList::resource_iterator end;

   ww = end = m_resources->resource_begin(guard) + (m_key % m_resources->size(guard));

   do {
      std::shared_ptr<Resource>& w = ResourceList::resource(ww);

      if (w->isAvailable() == true) {
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

