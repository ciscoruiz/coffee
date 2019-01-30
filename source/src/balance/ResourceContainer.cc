// MIT License
// 
// Copyright(c) 2018 Francisco Ruiz(francisco.ruiz.rayo@gmail.com)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
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
#include <coffee/balance/ResourceContainer.hpp>
#include <coffee/balance/GuardResourceContainer.hpp>

#include <mutex>

#include <coffee/basis/AsString.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TraceMethod.hpp>

#include <coffee/xml/Node.hpp>
#include <coffee/xml/Attribute.hpp>

#include <coffee/balance/SCCS.hpp>

using namespace coffee;

void balance::ResourceContainer::initialize()
   throw(basis::RuntimeException)
{
   LOG_THIS_METHOD();

   SCCS::activate();

   GuardResourceContainer guard(m_mutex);

   for(resource_iterator ii = resource_begin(guard), maxii = resource_end(guard); ii != maxii; ++ ii) {
      try {
         resource(ii)->initialize();
         LOG_DEBUG(resource(ii)->asString());
      }
      catch(basis::RuntimeException& ex) {
         LOG_ERROR(resource(ii)->getName() << " | " << ex.asString());
      }
   }

   if(m_resources.empty())
      LOG_WARN(asString() << " does not have any resource");

   if(countAvailableResources(guard) == 0)
      LOG_WARN(asString() << " does not have any available resource");
}

bool balance::ResourceContainer::add(std::shared_ptr<Resource> newResource)
   throw(basis::RuntimeException)
{
   logger::TraceMethod tm(logger::Level::Local7, COFFEE_FILE_LOCATION);

   if(!newResource) {
      COFFEE_THROW_EXCEPTION(asString() << " can not add an empty resource");
   }

   bool result = true;

   if(true) {
      GuardResourceContainer guard(m_mutex);

      for (auto& resource : m_resources) {
          if (resource->getName()  == newResource->getName()) {
              result = false;
              break;
          }
      }

      if(result == true) {
         m_resources.push_back(newResource);
      }
   }

   LOG_INFO(asString() << " | Add: " << newResource->asString() << " | Result=" << basis::AsString::apply(result));

   return result;
}

bool balance::ResourceContainer::remove(const std::string &resourceName)
    noexcept
{
    GuardResourceContainer guard(m_mutex);

    bool result = false;

    for (auto ii = m_resources.begin(), maxii = m_resources.end(); ii != maxii; ++ ii) {
        if (ResourceContainer::resource(ii)->getName() == resourceName) {
            m_resources.erase(ii);
            result = true;
            break;
        }
    }

    LOG_INFO(asString() << " | Remove: " << resourceName << " | Result=" << basis::AsString::apply(result));

    return result;
}

size_t balance::ResourceContainer::countAvailableResources(balance::GuardResourceContainer& guard) const
   noexcept
{
   size_t result = 0;

   for(const_resource_iterator ii = resource_begin(guard), maxii = resource_end(guard); ii != maxii; ++ ii) {
      if(resource(ii)->isAvailable())
         result ++;
   }

   return result;
}

balance::ResourceContainer::resource_iterator balance::ResourceContainer::next(balance::GuardResourceContainer& guard, resource_iterator ii)
   noexcept
{
   ii ++;

   if(ii == resource_end(guard))
      ii = resource_begin(guard);

   return ii;
}

//virtual
basis::StreamString balance::ResourceContainer::asString() const
   noexcept
{
   basis::StreamString result("balance.ResourceContainer { ");
   result += basis::NamedObject::asString();
   result += " | Available = ";

   int availableResources = 0;
   for (const_resource_iterator ii = m_resources.begin(), maxii = m_resources.end(); ii != maxii; ++ ii)
      availableResources += resource(ii)->isAvailable();

   result << availableResources << " of " <<  m_resources.size();
   return result.append(" }");
}

//virtual
std::shared_ptr<xml::Node> balance::ResourceContainer::asXML(std::shared_ptr<xml::Node>& parent) const
   throw(basis::RuntimeException)
{
   std::shared_ptr<xml::Node> result = parent->createChild(this->getName());

   for (const_resource_iterator ii = m_resources.begin(), maxii = m_resources.end(); ii != maxii; ++ ii)
      resource(ii)->asXML(result);

   return result;
}

