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

#include <mutex>

#include <coffee/balance/Resource.hpp>
#include <coffee/balance/ResourceContainer.hpp>
#include <coffee/balance/StrategyIndexed.hpp>
#include <coffee/balance/GuardResourceContainer.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TraceMethod.hpp>
#include <coffee/xml/Node.hpp>

using namespace coffee;

std::shared_ptr<balance::Resource> balance::StrategyIndexed::apply(const Request& request)
   throw (ResourceUnavailableException)
{
   logger::TraceMethod tm (logger::Level::Local7, COFFEE_FILE_LOCATION);

   GuardResourceContainer guard(m_resources);

   const int identifier = request.calculateIdentifier();

   if (m_resources->fullAvailableResources(guard)) {
      return apply(identifier, guard, m_resources);
   }

   auto availableResources = std::make_shared<balance::ResourceContainer>("AvailableResources");

   for (auto ii = m_resources->resource_begin(guard), maxii = m_resources->resource_end(guard); ii != maxii; ++ ii) {
      auto& resource = ResourceContainer::resource(ii);
      if (resource->isAvailable()) {
         availableResources->add(resource);
      }
   }

   return apply(identifier, guard, availableResources);
}

std::shared_ptr<xml::Node> balance::StrategyIndexed::asXML(std::shared_ptr<xml::Node>& parent) const
   throw(basis::RuntimeException)
{
   std::shared_ptr<xml::Node> result = parent->createChild("StrategyIndexed");

   m_resources->asXML(result);

   return result;
}

std::shared_ptr<balance::Resource> balance::StrategyIndexed::apply(const int identifier, GuardResourceContainer& guard, std::shared_ptr<ResourceContainer>& resourceContainer)
   throw (ResourceUnavailableException)
{
   if (resourceContainer->size(guard) == 0) {
      COFFEE_THROW_NAMED_EXCEPTION(ResourceUnavailableException, resourceContainer->getName() << " is empty");
   }

   auto result = ResourceContainer::resource(resourceContainer->resource_begin(guard) + (identifier % resourceContainer->size(guard)));

   if (!result) {
      COFFEE_THROW_NAMED_EXCEPTION(ResourceUnavailableException, this->asString() << " there is not any available resource");
   }

   LOG_LOCAL7("Result=" << result->asString());

   return result;

}
