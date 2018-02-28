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

#ifndef _coffee_app_Engine_h
#define _coffee_app_Engine_h

#include <vector>

#include <coffee/adt/RuntimeException.hpp>
#include <coffee/adt/StreamString.hpp>

#include <coffee/app/Runnable.hpp>

namespace coffee {

namespace xml {
class Node;
}

namespace app {

class Application;

/**
 * Every engine has a very clear task to do, and it is associated and managed by
 * some application.
 *
 * This way by passing the applicatio instance we will have access to all engines
 * associated to this application.
 *
 */
class Engine : public Runnable  {
public:
   /**
      Destructor.
   */
   virtual ~Engine() {;}

   /**
    * \return the application associated to this engine.
    */
   Application& getApplication() noexcept { return a_app; }

   /**
    * \return Logical name of this engine.
    */
   const std::string& getClassName() const noexcept { return getName(); }

   /**
    * \return Summarize information of this instance in a StreamString.
    */
   operator adt::StreamString() const noexcept { return asString(); }

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
    * Constructor.
    * \param app Application owner of this engine.
    * \param className Logical name of this engine.
    */
   Engine(Application& app, const char* className) :
      Runnable(className),
      a_app(app)
   {
   }

   /**
    * Set the name of a engine that must be initialized before this engine.
    * \param engineName Engine that must be initialized before this engine.
    * \warning It will not have any effect it this engine has been initialized before calling this method.
    */
   void addPredecessor(const char* engineName) noexcept;

   /**
    * This method should be implemented to specialize the initialization of this instance.
    * This method will be called from coffee::app::Application::start method.
    */
   void initialize() throw(adt::RuntimeException);

   /**
    * This method should be implemented to specialize the finalization of this instance.
    * This method will be called from coffee::app::Application::stopEngines method.
    */
   void stop() throw(adt::RuntimeException) { statusStopped(); do_stop(); }

private:
   typedef std::vector <std::string>::iterator iterator;

   Application& a_app;
   std::vector <std::string> a_predecessors;

   Engine(const Engine& other);
   iterator begin() noexcept { return a_predecessors.begin(); }
   iterator end() noexcept { return a_predecessors.end(); }
   const std::string& data(iterator ii) noexcept { return *ii; }

   virtual void do_initialize() throw(adt::RuntimeException) = 0;
   virtual void do_stop() throw(adt::RuntimeException) = 0;

   friend class Application;
};


}
}

#endif
