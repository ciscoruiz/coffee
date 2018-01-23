// COFFEE - COmpany eFFEEctive Platform
//
//(c) Copyright 2018 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/coffee
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#include "../../include/coffee/app/Engine.hpp"

#include <algorithm>

#include <coffee/logger/Logger.hpp>

#include <coffee/xml/Node.hpp>
#include <coffee/xml/Attribute.hpp>

#include <coffee/app/Application.hpp>

using namespace std;
using namespace coffee;

void app::Engine::addPredecessor(const char* engineName)
   noexcept
{
   const std::string name(engineName);

   if((std::find(begin(), end(), name)) != end())
      return;

   a_predecessors.push_back(name);

   LOG_DEBUG(asString() << " requires " << name);
}

void app::Engine::initialize()
   throw(adt::RuntimeException)
{
   statusStarting();

   try {
      for(iterator ii = begin(), maxii = end(); ii != maxii; ii ++) {
         const std::string& name = data(ii);

         Application::engine_iterator pp = a_app.engine_find(name.c_str());

         if(pp == a_app.engine_end()) {
            COFFEE_THROW_EXCEPTION(asString() << " requires '" << name << "' which was not defined");
         }

         auto predecessor = a_app.engine(pp);

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

adt::StreamString app::Engine::asString() const
   noexcept
{
   adt::StreamString result("app::Engine { ");
   result += Runnable::asString();
   return result += " }";
}

xml::Node& app::Engine::asXML(xml::Node& parent) const
   noexcept
{
   xml::Node& result = parent.createChild("app.Engine");

   Runnable::asXML(result);

   return result;
}

