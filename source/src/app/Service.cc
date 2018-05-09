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

#include <algorithm>

#include <coffee/app/Application.hpp>
#include <coffee/app/Service.hpp>
#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TraceMethod.hpp>
#include <coffee/xml/Attribute.hpp>
#include <coffee/xml/Node.hpp>

using namespace std;
using namespace coffee;

//static
const std::string app::Service::WhateverImplementation = "*";

void app::Service::waitEffectiveRunning()
   throw(basis::RuntimeException)
{
   LOG_THIS_METHOD();

   LOG_DEBUG(asString());

   if (!isStarting() && !isRunning()) {
      COFFEE_THROW_EXCEPTION(asString () << " could not wait for effective running");
   }

   effectiveRunning.wait();
}

void app::Service::notifyEffectiveRunning()
   noexcept
{
   LOG_THIS_METHOD();

   LOG_DEBUG(asString());

   effectiveRunning.signal();
}

void app::Service::require(const Feature::_v feature, const std::string& implementation)
   throw(basis::RuntimeException)
{
   auto ii = std::find_if(m_requirements.begin(), m_requirements.end(), [feature](const Requirement& ii) { return ii.m_feature == feature; });

   if (ii != m_requirements.end()) {
      COFFEE_THROW_EXCEPTION(asString() << " feature " << Feature::asString(feature) << " already registered");
   }

   Requirement requirement(feature, implementation);
   m_requirements.push_back(requirement);
   LOG_DEBUG(asString() << " requires " << requirement.asString());
}

std::shared_ptr<app::Service> app::Service::getRequirement(const Feature::_v feature)
   throw(basis::RuntimeException)
{
   auto ii = std::find_if(m_requirements.begin(), m_requirements.end(), [feature](const Requirement& ii) { return ii.m_feature == feature; });

   if (ii == m_requirements.end()) {
      COFFEE_THROW_EXCEPTION(asString() << " requirement " << Feature::asString(feature) << " was not registered");
   }

   if (!ii->m_selection) {
      COFFEE_THROW_EXCEPTION(asString() << " requirement " << Feature::asString(feature) << " no selection was done yet");
   }

   return ii->m_selection;
}

void app::Service::initialize()
   throw(basis::RuntimeException)
{
   statusStarting();

   try {
      for (auto& requirement : m_requirements) {
         requirement.m_selection = a_app.select(requirement.m_feature, requirement.m_implementation);

         if (requirement.m_selection->isRunning())
            continue;

         if (requirement.m_selection->isStarting()) {
            COFFEE_THROW_EXCEPTION(asString() << " has loop with requirement '" << requirement.m_selection->getName() << "'");
         }

         requirement.m_selection->initialize();
      }

      do_initialize();
      statusRunning();
   }
   catch(basis::RuntimeException&) {
      statusStopped();
      throw;
   }
}

basis::StreamString app::Service::asString() const
   noexcept
{
   basis::StreamString result("app.Service { ");
   result += Runnable::asString();
   result << ", Requirements=[";
   bool first = true;
   for (auto& requirement : m_requirements) {
      if (first == false)
         result << ",";
      result << requirement.asString();
      first = false;
   }
   result << "]";
   return result += " }";
}

std::shared_ptr<xml::Node> app::Service::asXML(std::shared_ptr<xml::Node>& parent) const
   throw(basis::RuntimeException)
{
   std::shared_ptr<xml::Node> result = parent->createChild("app.Service");
   Runnable::asXML(result);
   auto requirements = result->createChild("Requirements");
   for (auto requirement : m_requirements) {
      auto xmlNode = requirements->createChild("Requirement");
      xmlNode->createAttribute("Feature", Feature::asString(requirement.m_feature));

      if (requirement.m_implementation != Service::WhateverImplementation) {
         xmlNode->createAttribute("Implementation", requirement.m_implementation);
      }

      if (requirement.m_selection) {
         xmlNode->createAttribute("Selection", requirement.m_selection->getName());
      }
   }
   return result;
}

std::string app::Service::Requirement::asString() const
   noexcept
{
   basis::StreamString result("{ Feature=");

   result << Feature::asString(m_feature);

   if (m_implementation != Service::WhateverImplementation) {
      result << ",Implementation=" << m_implementation;
   }

   if (m_selection) {
      result << ",Selection=" << m_selection->getName();
   }

   return result << " }";
}
