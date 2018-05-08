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
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <coffee/app/Application.hpp>
#include <coffee/app/SCCS.hpp>
#include <coffee/app/Service.hpp>
#include <coffee/app/Feature.hpp>
#include <coffee/basis/RuntimeException.hpp>
#include <coffee/config/defines.hpp>
#include <coffee/config/Release.hpp>
#include <coffee/config/SCCSRepository.hpp>
#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TraceMethod.hpp>
#include <coffee/xml/Attribute.hpp>
#include <coffee/xml/Compiler.hpp>
#include <coffee/xml/Node.hpp>
#include <coffee/xml/SCCS.hpp>

using namespace std;
using namespace coffee;
using namespace coffee::basis;

//static
app::Application* app::Application::m_this = nullptr;

app::Application::Application(const char* shortName, const char* title, const char* version) :
   Runnable(shortName),
   a_version(std::string(version) + config::Release::getArchitecture()),
   a_title(title),
   a_pid(getpid()),
   a_outputContextFilename(basis::StreamString("/tmp/coffee.context.").append(AsString::apply(a_pid, "%05d")).append(".xml"))
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
   m_services.clear();
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
   catch(basis::RuntimeException& ex) {
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
   catch(basis::RuntimeException& ex) {
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

   for(auto ii = m_services.begin(); ii != m_services.end(); ii ++) {
      LOG_INFO("Starting service | " <<  ii->second->asString());
      ii->second->initialize();
   }
}

// virtual
void app::Application::do_stop()
   throw(RuntimeException)
{
   LOG_THIS_METHOD();

   basis::StreamString ss("Some services do not accept the request stop { ");
   bool exception = false;

   for(auto ii = m_services.begin(), maxii = m_services.end(); ii != maxii; ii ++) {
      auto& service = ii->second;

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

bool app::Application::attach(std::shared_ptr<Service> service)
   throw(RuntimeException)
{
   if(!service)
      COFFEE_THROW_EXCEPTION("Can not associate a NULL service");

   const Feature::_v feature = service->getFeature();

   auto range = m_services.equal_range(feature);

   const std::string& implementation = service->getImplementation();

   while(range.first != range.second) {
      if (range.first->second->getImplementation() == implementation) {
         LOG_INFO(service->asString() << " | Already defined");
         return false;
      }
      ++ range.first;
   }

   LOG_DEBUG("Registering " << service->asString());

   m_services.insert(Services::value_type(feature, service));

   if(isRunning()) {
      LOG_INFO("Initializing service | " << service->asString());
      service->initialize();
   }

   return true;
}

std::shared_ptr<app::Service> app::Application::select(const app::Feature::_v feature, const std::string& implementation)
   throw(basis::RuntimeException)
{
   auto range = m_services.equal_range(feature);

   if (range.first == range.second) {
      COFFEE_THROW_EXCEPTION(getName() << " does not have any service for feature=" << Feature::asString(feature));
   }

   if (implementation != Service::WhateverImplementation) {
      while(range.first != range.second) {
         if (range.first->second->getImplementation() == implementation) {
            return range.first->second;
         }
         ++ range.first;
      }

      COFFEE_THROW_EXCEPTION(getName() << " has service for feature=" << Feature::asString(feature) << "but no implementation=" << implementation);
   }

   return range.first->second;
};

void app::Application::writeContext(const boost::filesystem::path& file)
   throw(RuntimeException)
{
   ofstream out;

   out.open(file.c_str());

   if(out.is_open() == false) {
      COFFEE_THROW_EXCEPTION("Can not open file'" << file.c_str() << "'");
   }

   LOG_NOTICE("File: " << file.c_str());

   try {
      std::shared_ptr<xml::Node> root = std::make_shared<xml::Node>("Context");
      asXML(root);
      xml::Compiler compiler;
      out << compiler.apply(root) << std::endl;
      out.close();
   }
   catch(const basis::RuntimeException& ex) {
      logger::Logger::write(ex);
   }
}

basis::StreamString app::Application::asString() const noexcept
{
   basis::StreamString result("app::Application { ");
   result << app::Runnable::asString();
   result << " | #services=" << m_services.size();
   return result += " }";
}

std::shared_ptr<xml::Node> app::Application::asXML(std::shared_ptr<xml::Node>& root) const
   throw(basis::RuntimeException)
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
   for(auto ii = m_services.begin(), maxii = m_services.end(); ii != maxii; ii ++)
      ii->second->asXML(services);

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
   catch(basis::RuntimeException& ex) {
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
   catch(basis::RuntimeException& ex) {
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
