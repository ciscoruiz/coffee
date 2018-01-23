// COFFEE - COmpany eFFEEctive Platform
//
// (c) Copyright 2018 Francisco Ruiz Rayo
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <unistd.h>
#include <stdio.h>

#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <coffee/config/Release.hpp>
#include <coffee/config/SCCSRepository.hpp>
#include <coffee/config/defines.hpp>

#include <coffee/adt/RuntimeException.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TraceMethod.hpp>

#include <coffee/xml/Node.hpp>
#include <coffee/xml/Attribute.hpp>
#include <coffee/xml/Compiler.hpp>
#include <coffee/xml/SCCS.hpp>

#include <coffee/app/Application.hpp>
#include <coffee/app/SCCS.hpp>
#include <coffee/app/Engine.hpp>

using namespace std;
using namespace coffee;
using namespace coffee::adt;

//static
app::Application* app::Application::m_this = nullptr;

app::Application::Application (const char* shortName, const char* title, const char* version, const char* date, const char* time) :
   Runnable (shortName),
   a_version (version),
   a_title (title),
   a_enableGPL (false)
{
   sigset (SIGUSR1, handlerUserSignal);
   sigset (SIGUSR2, handlerUserSignal);
   sigset (SIGTERM, handlerSignalTerminate);
   sigignore (SIGINT);

   app::SCCS::activate ();
   xml::SCCS::activate();

   a_version += config::Release::getArchitecture ();

   a_pid = getpid ();

   if (m_this == nullptr)
      m_this = this;

   cout << getName () << " - " << a_title << ". Version " << a_version << endl;
   if (date || time) {
      cout << "Release date: ";
      if (date) cout << date << " ";
      if (time) cout << time;
      cout << endl;
   }
   cout << "(c) Copyright 2018 by Francisco Ruiz." << endl << endl;
}

app::Application::~Application()
{
   if (m_this == this) {
      m_this = nullptr;
   }
   a_engines.clear();
}

app::Application::engine_iterator app::Application::engine_find (const char* className)
   noexcept
{
   for (engine_iterator ii = engine_begin (), maxii = engine_end (); ii != maxii; ii ++) {
      auto engine = Application::engine (ii);
      if (coffee_strcmp (className, engine->getClassName ()) == 0)
         return ii;
   }

   return engine_end();
}

/**
 * (1) Si se ejecuta el metodo clone evita que los hijos que termina se queden como zombies.
 */
void app::Application::start ()
   throw (RuntimeException)
{
   LOG_THIS_METHOD ();

   config::SCCSRepository& moduleManager = config::SCCSRepository::getInstance();

   if (isRunning() == true) {
      COFFEE_THROW_EXCEPTION(asString () << " | Already running");
   }

   if (sigset (SIGCHLD, handlerChildTerminate) != 0) {
      LOG_ERROR (asString () << " | Can not install child termination handler | Error=" << strerror (errno) );
   }

   try {
      if (a_enableGPL == true) {
         cout << "This program is free software: you can redistribute it and/or modify" << endl;
         cout << "it under the terms of the GNU General Public License as published by" << endl;
         cout << "the Free Software Foundation, either version 3 of the License, or" << endl;
         cout << "(at your option) any later version." << endl << endl;

         cout << "This program is distributed in the hope that it will be useful," << endl;
         cout << "but WITHOUT ANY WARRANTY; without even the implied warranty of" << endl;
         cout << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the" << endl;
         cout << "GNU General Public License for more details." << endl << endl;

         cout << "You should have received a copy of the GNU General Public License" << endl;
         cout << "along with this program.  If not, see <http://www.gnu.org/licenses/>." << endl << endl;
      }

      cout << "COFFEE - COmpany eFFEEctive Platform. Version " << config::Release::getVersion () << endl;
      cout << "Developed by Francisco Ruiz" << endl;
      cout << "Release date: " << __DATE__ << " " << __TIME__ << endl;
      cout << "(c) Copyright 2018,2014 by Francisco Ruiz." << endl << endl;

      initialize ();

      cout << "Loading modules ...." << endl;
      for (config::SCCSRepository::const_entry_iterator ii = moduleManager.entry_begin (), maxii = moduleManager.entry_end (); ii != maxii; ++ ii) {
         cout << "\t Module " << config::SCCSRepository::module_name (ii) << endl;
      }
      cout << endl;

      statusStarting();

      startEngines();

      statusRunning();
      run ();
      statusStopped();

      stopEngines ();
   }
   catch (adt::RuntimeException& ex) {
      logger::Logger::write(ex);
      statusStopped();
      stopEngines ();
      throw;
   }

   cout << getName () << " finished ..." << endl << endl;
}

void app::Application::startEngines ()
   throw (RuntimeException)
{
   LOG_THIS_METHOD ();

   for (engine_iterator ii = engine_begin (); ii != engine_end (); ii ++) {
      auto engine = Application::engine (ii);
      LOG_INFO ("Initializing engine | " <<  engine->asString ());
      engine->initialize ();
   }
}

void app::Application::stopEngines ()
   noexcept
{
   LOG_THIS_METHOD ();

   for (engine_iterator ii = engine_begin (); ii != engine_end (); ii ++) {
      auto engine = Application::engine (ii);

      if (engine->isStopped() == true)
         continue;

      LOG_INFO ("Finalizing engine | " <<  engine->asString ());

      try {
         engine->stop ();
      }
      catch (RuntimeException& ex) {
         logger::Logger::write (ex);
      }
   }
}

/**
 * Se invocar� porque se haya recibido la se�al de parada o porque alg�n enginee de la aplicaci�n
 * haya solicitado la parada de la aplicaci�n.
 */
// virtual
void app::Application::do_requestStop ()
   throw (RuntimeException)
{
   LOG_THIS_METHOD ();

   for (engine_iterator ii = engine_begin (), maxii = engine_end (); ii != maxii; ii ++) {
      auto engine = Application::engine (ii);

      LOG_INFO ("Request stop for engine | " <<  engine->asString ());

      try {
         engine->requestStop ();
      }
      catch (RuntimeException& ex) {
         logger::Logger::write (ex);
         exit (-1);
      }
   }
}

//---------------------------------------------------------------------------------------
// (1) Se invoca desde los constructores de los enginees => no se puede invocar a
// ningn m�odo virtual => obliga a que est� definidos antes de comenzar la
// ejecucin de la aplicacin => se inicilizar� en el Application::start ya que en
// ese momento los objetos est� completamente creados.
//---------------------------------------------------------------------------------------
void app::Application::attach (std::shared_ptr<Engine> engine)
   throw (RuntimeException)
{
   if (!engine)
      COFFEE_THROW_EXCEPTION ("Can not associate a NULL engine");

   auto ii = engine_find(engine->getName());

   if (ii != engine_end()) {
      LOG_INFO (engine->asString () << " | Already defined") ;
      return;
   }

   LOG_DEBUG ("Registering " << engine->asString ());

   a_engines.push_back (engine);

   if (isRunning ()) {
      LOG_INFO ("Initializing engine | " << engine->asString ());
      engine->initialize ();
   }
}

void app::Application::writeContext (const std::string& file)
   throw (RuntimeException)
{
   ofstream out;

   out.open (file.c_str ());

   if (out.is_open () == false) {
      COFFEE_THROW_EXCEPTION("Can not open file'" << file << "'");
   }

   LOG_NOTICE ("File: " << file);

   xml::Node root ("Context");

   asXML (root);

   xml::Compiler compiler;

   out << compiler.apply (root) << std::endl;

   out.close ();
}

adt::StreamString app::Application::asString() const noexcept
{
   adt::StreamString result("app::Application { ");
   result << app::Runnable::asString();
   result << " | #engines=" << a_engines.size();
   return result += " }";
}

xml::Node& app::Application::asXML (xml::Node& root) const
   noexcept
{
   xml::Node& app = root.createChild ("app.Application");

   app.createAttribute ("Version", getVersion ());
   app.createAttribute ("pid", getPid ());

   config::SCCSRepository& moduleManager = config::SCCSRepository::getInstance();

   xml::Node& modules = app.createChild ("app.Modules");
   for (config::SCCSRepository::const_entry_iterator ii = moduleManager.entry_begin (), maxii = moduleManager.entry_end (); ii != maxii; ii ++) {
      xml::Node& xmlModule = modules.createChild ("Module");
      xmlModule.createAttribute ("Name", config::SCCSRepository::module_name (ii));
   }

   xml::Node& engines = app.createChild ("app.Engines");
   for (const_engine_iterator ii = engine_begin (), maxii = engine_end (); ii != maxii; ii ++)
      engine (ii)->asXML (engines);

   return app;
}

// virtual
void app::Application::signalUSR1 ()
   throw (RuntimeException)
{
   adt::StreamString fileName ("/var/tmp/nemesis.context.");
   writeContext (fileName << AsString::apply (getPid (), "%05d"));
}

// virtual
void app::Application::signalUSR2 ()
   throw (RuntimeException)
{
   logger::Level::_v original = logger::Logger::getLevel ();

   int currentLevel = (int) original + 1;

   if (currentLevel > logger::Level::Debug)
      currentLevel = logger::Level::Error;

   logger::Logger::setLevel ((logger::Level::_v) currentLevel);

   string msg ("Do not worry about level of this line | Original level: ");
   msg += logger::Level::enumName (original);
   msg += " | Current level: ";
   msg += logger::Level::enumName (logger::Logger::getLevel());

   logger::Logger::write (logger::Logger::getLevel(), msg, COFFEE_FILE_LOCATION);
}

// static
void app::Application::handlerUserSignal (int signalID)
   noexcept
{
   try {
      if (m_this != nullptr) {
         if (signalID == SIGUSR1)
            m_this->signalUSR1 ();
         else
            m_this->signalUSR2 ();
      }
   }
   catch (adt::RuntimeException& ex) {
      logger::Logger::write (ex);
   }
}

// static
void app::Application::handlerSignalTerminate (int)
   noexcept
{
   try {
      if (m_this != nullptr) {
         LOG_WARN (m_this->asString () << " | Received SIGTERM signal");
         m_this->requestStop ();
      }
   }
   catch (adt::RuntimeException& ex) {
      logger::Logger::write (ex);
      exit (EXIT_SUCCESS);
   }
}

/**
 * Se recoge el estado de terminacion del hijo para que no se quede considerado como un zombie y se
 * elimina de la lista de procesos hijos.
 */
// static
void app::Application::handlerChildTerminate (int sig)
   noexcept
{
   if (sig == SIGCHLD) {
      int status;
      pid_t pid = wait (&status);
   }
}
