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

#include <coffee/logger/Logger.hpp>

#include <coffee/xml/Node.hpp>
#include <coffee/xml/Attribute.hpp>

#include <coffee/app/Application.hpp>
#include <coffee/app/Service.hpp>

using namespace std;
using namespace coffee;

void app::Service::addPredecessor(const char* engineName)
   noexcept
{
   const std::string name(engineName);

   if((std::find(begin(), end(), name)) != end())
      return;

   a_predecessors.push_back(name);

   LOG_DEBUG(asString() << " requires " << name);
}

void app::Service::initialize()
   throw(adt::RuntimeException)
{
   statusStarting();

   try {
      for(iterator ii = begin(), maxii = end(); ii != maxii; ++ ii) {
         const std::string& name = data(ii);

         Application::service_iterator pp = a_app.service_find(name.c_str());

         if(pp == a_app.service_end()) {
            COFFEE_THROW_EXCEPTION(asString() << " requires '" << name << "' which was not defined");
         }

         auto predecessor = a_app.service(pp);

         if(predecessor->isRunning() == true) {
            LOG_DEBUG("Predecessor '" << name << "' already running");
            continue;
         }

         if(predecessor->isStarting()) {
            COFFEE_THROW_EXCEPTION(asString() << " has loop with requirement '" << name << "'");
         }

         predecessor->initialize();
      }

      do_initialize();
      statusRunning();
   }
   catch(adt::RuntimeException&) {
      statusStopped();
      throw;
   }
}

adt::StreamString app::Service::asString() const
   noexcept
{
   adt::StreamString result("app::Engine { ");
   result += Runnable::asString();
   return result += " }";
}

std::shared_ptr<xml::Node> app::Service::asXML(std::shared_ptr<xml::Node>& parent) const
   noexcept
{
   std::shared_ptr<xml::Node> result = parent->createChild("app.Engine");
   Runnable::asXML(result);
   return result;
}

