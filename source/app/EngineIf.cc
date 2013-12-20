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
#include <algorithm>

#include <wepa/logger/Logger.hpp>

#include <wepa/app/EngineIf.hpp>
#include <wepa/app/functions.hpp>

#include <wepa/xml/Node.hpp>
#include <wepa/xml/Attribute.hpp>

#include <wepa/app/Application.hpp>

using namespace std;
using namespace wepa;

app::EngineIf::EngineIf (Application& app, const char* className) :
   Runnable (className),
   a_app (app)
{
   if (app.isRunning () == false) {
      try {
         app.attach (this);
      }
      catch (adt::RuntimeException& ex) {
         logger::Logger::write (ex);
      }
   }
   else {
      LOG_WARN(asString () << " | Application was started before initialize this engine");
   }
}

void app::EngineIf::addPredecessor (const char* engineName)
   throw ()
{
   const std::string name (engineName);

   if ((std::find (begin (), end (), name)) != end ())
      return;

   a_predecessors.push_back (name);

   LOG_DEBUG (asString () << " requires " << name);
}

void app::EngineIf::initialize ()
   throw (adt::RuntimeException)
{
   statusStarting ();

   EngineIf* predecessor;

   try {
      for (iterator ii = begin (), maxii = end (); ii != maxii; ii ++) {
         const std::string& name = data (ii);

         if ((predecessor = a_app.engine_find (name.c_str ())) == NULL) {
            WEPA_THROW_EXCEPTION(asString () << " requires '" << name << "' which was not defined");
         }

         if (predecessor->isRunning() == true) {
            LOG_DEBUG ("Predecessor '" << name << "' already running");
            continue;
         }

         if (predecessor->isStarting()) {
            WEPA_THROW_EXCEPTION(asString () << " has loop with requirement '" << name << "'");
         }

         predecessor->initialize ();
      }

      do_initialize ();
      statusRunning();
   }
   catch (adt::RuntimeException&) {
      statusStopped ();
      throw;
   }
}

adt::StreamString  app::EngineIf::asString () const
   throw ()
{
   adt::StreamString result ("app::EngineIf { ");
   result += Runnable::asString ();
   return result += " }";
}

xml::Node& app::EngineIf::asXML (xml::Node& parent) const
   throw ()
{
   xml::Node& result = parent.createChild ("app.Engine");

   Runnable::asXML (result);

   return result;
}

