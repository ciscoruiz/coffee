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


#ifndef _coffee_app_Application_hpp_
#define _coffee_app_Application_hpp_

#include <vector>
#include <memory>

#include <boost/filesystem.hpp>

#include <coffee/adt/RuntimeException.hpp>

#include <coffee/app/Runnable.hpp>

namespace coffee {

namespace xml {
class Node;
}

namespace app {

class Service;

/**
 * This abstraction manage all resources needed to run our application.
 *
 * It maintains the list of services needed to run our application.
 *
 * \include Application_test.cc
*/
class Application : public Runnable {
public:
   typedef std::vector<std::shared_ptr<Service> > Services;
   typedef Services::iterator service_iterator;
   typedef Services::const_iterator const_service_iterator;

   /**
      Constructor.

      @param shortName Logical name.
      @param title Title of this application. It will be shown during run up.
      @param version version of this application.
   */
   Application(const char* shortName, const char* title, const char* version);

   /**
      Destructor.
   */
   virtual ~Application();

   /**
    * \return the logical name of this application.
    */
   const std::string& getShortName() const noexcept { return getName(); }

   /**
    * \return The version of this application.
    */
   const std::string& getVersion() const noexcept { return a_version; }

   /**
    * \return the title of this application.
    */
   const std::string& getTitle() const noexcept { return a_title; }

   /**
    * \return the pid assigned by the operating system to the proccess running this application.
    */
   pid_t getPid() const noexcept { return a_pid; }

   /**
    * The the path where it would write the context.
    */
   void setOutputContextFilename(const boost::filesystem::path& outputContextFilename) noexcept { this->a_outputContextFilename = outputContextFilename; }

   /**
    * \return The context path where it would write the context.
    */
   const boost::filesystem::path& getOutputContextFilename() const noexcept { return a_outputContextFilename; }

   /**
    * \return the service_iterator addresses the service received as parameter or #service_end if the name is not found.
    */
   service_iterator service_find(const std::string& serviceName) noexcept;

   /**
    * Initialize all resources related to this application, and then it will call virtual method #initialize and #run.
    */
   void start() throw(adt::RuntimeException);

   /**
    * Attach the service to this application. It will be started before this application start to run.
    */
   virtual void attach(std::shared_ptr<Service> service) throw(adt::RuntimeException);

   /**
    * \return service_iterator to the first attached service.
    */
   service_iterator service_begin() noexcept { return a_services.begin(); }

   /**
    * \return service_iterator to the last attached service.
    */
   const_service_iterator service_end() const noexcept { return a_services.end(); }

   /**
    * \return the service addressed by the service_iterator.
    * \warning the value ii must be contained in [#service_begin, #service_end)
    */
   static std::shared_ptr<Service>& service(service_iterator ii) noexcept { return std::ref(*ii); }

   /**
    * \return service_iterator to the first attached service.
    */
   const_service_iterator service_begin() const noexcept { return a_services.begin(); }

   /**
    * \return service_iterator to the last attached service.
    */
   service_iterator service_end() noexcept { return a_services.end(); }

   /**
    * \return the service addressed by the service_iterator.
    * \warning the value ii must be contained in [#service_begin, #service_end)
    */
   static const std::shared_ptr<Service> service(const_service_iterator ii) noexcept { return std::ref(*ii); }

   /**
    * Write the context as a coffee::xml::Node into the path indicated by file.
    */
   void writeContext(const boost::filesystem::path& file) throw(adt::RuntimeException);

   /**
    * This virtual method will give us the opportunity to initialize custom resources of
    * our inherit application.
   */
   virtual void initialize() throw(adt::RuntimeException) {;}

   /**
    * \return Summarize information of this instance in a StreamString.
    */
   virtual adt::StreamString asString() const noexcept;

   /**
    * \return Summarize information of this instance in a coffee::xml::Node.
    */
   virtual std::shared_ptr<xml::Node> asXML(std::shared_ptr<xml::Node>& parent) const noexcept;

protected:
   /**
    * Virtual method to implement for running our application.
    */
   virtual void run() throw(adt::RuntimeException) = 0;

   /**
    * Virtual method to capture the request stop.
    */
   virtual void do_requestStop() throw(adt::RuntimeException);

   /**
    * Handler for signal USR1, it will write the context into file #getOutputContextFilename.
    */
   virtual void signalUSR1() throw(adt::RuntimeException);

   /**
    * Handler for signal USR2
    * By default it will change the trace level, adding 1 to the current level. Once it reach
    * the level logger::Level::Debug it will return to the logger::Level::Error.
    */
   virtual void signalUSR2() throw(adt::RuntimeException);

private:
   static Application* m_this;

   const std::string a_version;
   const std::string a_title;
   const pid_t a_pid;
   boost::filesystem::path a_outputContextFilename;
   Services a_services;

   void startServices() throw(adt::RuntimeException);
   void stopServices() noexcept;
   void sendSignalToChilds(const int signal) noexcept;

   static void handlerUserSignal(int) noexcept;
   static void handlerSignalTerminate(int) noexcept;
   static void handlerChildTerminate(int sig) noexcept;
};

}
}

#endif

