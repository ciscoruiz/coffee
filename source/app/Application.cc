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

#include <iostream>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <cctype>

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
#include <coffee/app/Service.hpp>

using namespace std;
using namespace coffee;
using namespace coffee::adt;

//static
app::Application* app::Application::m_this = nullptr;

app::Application::Application(const char* shortName, const char* title, const char* version) :
   Runnable(shortName),
   a_version(std::string(version) + config::Release::getArchitecture()),
   a_title(title),
   a_pid(getpid()),
   a_outputContextFilename(adt::StreamString("/tmp/coffee.context.").append(AsString::apply(a_pid, "%05d")).append(".xml"))
{
   sigset(SIGUSR1, handlerUserSignal);
   sigset(SIGUSR2, handlerUserSignal);
   sigset(SIGTERM, handlerSignalTerminate);
   sigignore(SIGINT);

   app::SCCS::activate();
   xml::SCCS::activate();

   if(m_this == nullptr)
      m_this = this;

}

app::Application::~Application()
{
   if(m_this == this) {
      m_this = nullptr;
   }
   a_services.clear();
}

app::Application::service_iterator app::Application::service_find(const std::string& serviceName)
   noexcept
{
   for(service_iterator ii = service_begin(), maxii = service_end(); ii != maxii; ii ++) {
      auto service = Application::service(ii);
      if(service->getClassName() == serviceName)
         return ii;
   }

   return service_end();
}

void app::Application::start()
   throw(RuntimeException)
{
   LOG_THIS_METHOD();

   std::string upperName = getName();
   std::transform(upperName.begin(), upperName.end(), upperName.begin(), [](unsigned char c){return std::toupper(c);});
   const bool isAnApplicationTest = (upperName.find("TEST") != std::string::npos);

   config::SCCSRepository& moduleManager = config::SCCSRepository::getInstance();

   if(isRunning() == true) {
      COFFEE_THROW_EXCEPTION(asString() << " | Already running");
   }

   if(sigset(SIGCHLD, handlerChildTerminate) != 0) {
      LOG_ERROR(asString() << " | Can not install child termination handler | Error=" << strerror(errno) );
   }

   try {
      if (!isAnApplicationTest) {
         cout << getName() << " - " << a_title << ". Version " << a_version << endl;
         cout << "COFFEE - COmpany eFFEEctive platform. " << config::Release::getVersion() << endl;
         cout << "Release date: " << __DATE__ << " " << __TIME__ << endl;
         cout << "(c) Copyright 2018,2014 by Francisco Ruiz." << endl << endl;
      }
      initialize();
   }
   catch(adt::RuntimeException& ex) {
      logger::Logger::write(ex);
      throw;
   }

   if (!isAnApplicationTest) {
      cout << "Loading modules ...." << endl;
      for(config::SCCSRepository::const_entry_iterator ii = moduleManager.entry_begin(), maxii = moduleManager.entry_end(); ii != maxii; ++ ii) {
         cout << "\t Module " << config::SCCSRepository::module_name(ii) << endl;
      }
      cout << endl;
   }

   try {
      statusStarting();
      startServices();
      statusRunning();
      run();
   }
   catch(adt::RuntimeException& ex) {
      logger::Logger::write(ex);
      stop();
      throw;
   }

   stop();

   if (!isAnApplicationTest) {
      cout << getName() << " finished ..." << endl << endl;
   }
}

void app::Application::startServices()
   throw(RuntimeException)
{
   LOG_THIS_METHOD();

   for(service_iterator ii = service_begin(); ii != service_end(); ii ++) {
      auto service = Application::service(ii);
      LOG_INFO("Starting service | " <<  service->asString());
      service->initialize();
   }
}

// virtual
void app::Application::do_stop()
   throw(RuntimeException)
{
   LOG_THIS_METHOD();

   adt::StreamString ss("Some services do not accept the request stop { ");
   bool exception = false;

   for(service_iterator ii = service_begin(), maxii = service_end(); ii != maxii; ii ++) {
      auto service = Application::service(ii);

      LOG_INFO("Send stop for service | " <<  service->asString());

      try {
         service->stop();
      }
      catch(RuntimeException& ex) {
         logger::Logger::write(ex);
         ss << service->getName() << " ";
         exception = true;
      }
   }

   if (exception) {
      ss << "}";
      COFFEE_THROW_EXCEPTION(ss);
   }
}

void app::Application::attach(std::shared_ptr<Service> service)
   throw(RuntimeException)
{
   if(!service)
      COFFEE_THROW_EXCEPTION("Can not associate a NULL service");

   auto ii = service_find(service->getName());

   if(ii != service_end()) {
      LOG_INFO(service->asString() << " | Already defined") ;
      return;
   }

   LOG_DEBUG("Registering " << service->asString());

   a_services.push_back(service);

   if(isRunning()) {
      LOG_INFO("Initializing service | " << service->asString());
      service->initialize();
   }
}

void app::Application::writeContext(const boost::filesystem::path& file)
   throw(RuntimeException)
{
   ofstream out;

   out.open(file.c_str());

   if(out.is_open() == false) {
      COFFEE_THROW_EXCEPTION("Can not open file'" << file.c_str() << "'");
   }

   LOG_NOTICE("File: " << file.c_str());

   std::shared_ptr<xml::Node> root = std::make_shared<xml::Node>("Context");

   asXML(root);

   xml::Compiler compiler;

   out << compiler.apply(root) << std::endl;

   out.close();
}

adt::StreamString app::Application::asString() const noexcept
{
   adt::StreamString result("app::Application { ");
   result << app::Runnable::asString();
   result << " | #services=" << a_services.size();
   return result += " }";
}

std::shared_ptr<xml::Node> app::Application::asXML(std::shared_ptr<xml::Node>& root) const
   noexcept
{
   std::shared_ptr<xml::Node> result = root->createChild("app.Application");

   result->createAttribute("Version", getVersion());
   result->createAttribute("pid", getPid());

   config::SCCSRepository& moduleManager = config::SCCSRepository::getInstance();

   std::shared_ptr<xml::Node> modules = result->createChild("Modules");
   for(config::SCCSRepository::const_entry_iterator ii = moduleManager.entry_begin(), maxii = moduleManager.entry_end(); ii != maxii; ii ++) {
      std::shared_ptr<xml::Node> xmlModule = modules->createChild("Module");
      xmlModule->createAttribute("Name", config::SCCSRepository::module_name(ii));
   }

   std::shared_ptr<xml::Node> services = result->createChild("Services");
   for(const_service_iterator ii = service_begin(), maxii = service_end(); ii != maxii; ii ++)
      service(ii)->asXML(services);

   return result;
}

// virtual
void app::Application::signalUSR1()
   throw(RuntimeException)
{
   writeContext(a_outputContextFilename);
}

// virtual
void app::Application::signalUSR2()
   throw(RuntimeException)
{
   logger::Level::_v original = logger::Logger::getLevel();

   int currentLevel =(int) original + 1;

   if(currentLevel > logger::Level::Debug)
      currentLevel = logger::Level::Error;

   logger::Logger::setLevel((logger::Level::_v) currentLevel);

   string msg("Do not worry about level of this line | Original level: ");
   msg += logger::Level::enumName(original);
   msg += " | Current level: ";
   msg += logger::Level::enumName(logger::Logger::getLevel());

   logger::Logger::write(logger::Logger::getLevel(), msg, COFFEE_FILE_LOCATION);
}

// static
void app::Application::handlerUserSignal(int signalID)
   noexcept
{
   try {
      if(m_this != nullptr) {
         if(signalID == SIGUSR1)
            m_this->signalUSR1();
         else
            m_this->signalUSR2();
      }
   }
   catch(adt::RuntimeException& ex) {
      logger::Logger::write(ex);
   }
}

// static
void app::Application::handlerSignalTerminate(int)
   noexcept
{
   try {
      if(m_this != nullptr) {
         LOG_WARN(m_this->asString() << " | Received SIGTERM signal");
         m_this->stop();
      }
   }
   catch(adt::RuntimeException& ex) {
      logger::Logger::write(ex);
      exit(EXIT_SUCCESS);
   }
}

// static
void app::Application::handlerChildTerminate(int sig)
   noexcept
{
   if(sig == SIGCHLD) {
      int status;
      pid_t pid = wait(&status);
   }
}
