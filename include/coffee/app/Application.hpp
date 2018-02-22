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

#include <coffee/adt/RuntimeException.hpp>

#include <coffee/app/Runnable.hpp>

namespace coffee {

namespace xml {
class Node;
}

namespace app {

class Engine;

/**
 * This abstraction manage all resources needed to run our application.
 *
 * It maintains the list of engines needed to run our application.
*/
class Application : public Runnable {
public:
   typedef std::vector<std::shared_ptr<Engine> > Engines;
   typedef Engines::iterator engine_iterator;
   typedef Engines::const_iterator const_engine_iterator;

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
      Devuelve la version indicado en el contructor de esta aplicacion.
      \return La version indicado en el contructor de esta aplicacion.
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
    * \return the engine_iterator addresses the engine received as parameter or #engine_end if the name is not found.
    */
   engine_iterator engine_find(const std::string& className) noexcept;

   /**
    * Initialize all resources related to this application, and then it will call virtual method #initialize and #run.
    */
   void start() throw(adt::RuntimeException);

   /**
    * Attach the engine to this application. It will be started before this application start to run.
    */
   virtual void attach(std::shared_ptr<Engine> engine) throw(adt::RuntimeException);

   /**
    * \return engine_iterator the first attached engine.
    */
   engine_iterator engine_begin() noexcept { return a_engines.begin(); }

   /**
    * \return engine_iterator the last attached engine.
    */
   const_engine_iterator engine_end() const noexcept { return a_engines.end(); }

   /**
    * \return the engine addressed by the engine_iterator.
    * \warning the value ii must be contained in [#engine_begin, #engine_end)
    */
   static std::shared_ptr<Engine>& engine(engine_iterator ii) noexcept { return std::ref(*ii); }

   /**
    * \return engine_iterator the first attached engine.
    */
   const_engine_iterator engine_begin() const noexcept { return a_engines.begin(); }

   /**
    * \return engine_iterator the last attached engine.
    */
   engine_iterator engine_end() noexcept { return a_engines.end(); }

   /**
    * \return the engine addressed by the engine_iterator.
    * \warning the value ii must be contained in [#engine_begin, #engine_end)
    */
   static const std::shared_ptr<Engine> engine(const_engine_iterator ii) noexcept { return std::ref(*ii); }

   /**
    * Write the context as a coffee::xml::Node into the path indicated by file.
    */
   void writeContext(const std::string& file) throw(adt::RuntimeException);

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
    * Handler for signal USR1
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

   std::string a_version;
   const std::string a_title;
   Engines a_engines;
   pid_t a_pid;

   void startEngines() throw(adt::RuntimeException);
   void stopEngines() noexcept;
   void sendSignalToChilds(const int signal) noexcept;

   static void handlerUserSignal(int) noexcept;
   static void handlerSignalTerminate(int) noexcept;
   static void handlerChildTerminate(int sig) noexcept;
};

}
}

#endif

