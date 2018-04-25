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

#ifndef _coffee_app_Service_h
#define _coffee_app_Service_h

#include <vector>

#include <coffee/app/Feature.hpp>
#include <coffee/app/Runnable.hpp>
#include <coffee/basis/RuntimeException.hpp>
#include <coffee/basis/Semaphore.hpp>
#include <coffee/basis/StreamString.hpp>

namespace coffee {

namespace xml {
class Node;
}

namespace app {

class Application;

/**
 * Every Service has a very clear task to do, and it is associated and managed by
 * some application.
 *
 * This way by passing the application instance we will have access to all services
 * associated to this application.
 */
class Service : public Runnable  {
public:
   static const std::string WhateverImplementation;

   /**
      Destructor.
   */
   virtual ~Service() {;}

   /**
    * \return the application associated to this service.
    */
   Application& getApplication() noexcept { return a_app; }

   /**
    * @return Feature offered by this service
    */
   const Feature::_v getFeature() const noexcept { return m_feature; }

   /**
    * @return Implementation used for this service for offering the feature
    */
   const std::string& getImplementation() const noexcept { return m_implementation; }

   /**
    * Method to call to be sure Service is fully prepared to accept requests
    * \include time/TimeFixture.hpp
    */
   void waitEffectiveRunning() throw(basis::RuntimeException);

   /**
    * Method that the server should call from some other thread to notify
    * it is prepared to accept request.
    */
   void notifyEffectiveRunning() noexcept;

   /**
    * \return The service selected to offer the feature received as parameter.
    * \warning This method will no have an effective value until the application starts to run.
    */
   std::shared_ptr<Service> getRequirement(const Feature::_v feature) throw(basis::RuntimeException);

   /**
    * \return Summarize information of this instance in a StreamString.
    */
   operator basis::StreamString() const noexcept { return asString(); }

   /**
    * \return Summarize information of this instance in a StreamString.
    */
   virtual basis::StreamString asString() const noexcept;

   /**
    * \return Summarize information of this instance in a coffee::xml::Node.
    */
   virtual std::shared_ptr<xml::Node> asXML(std::shared_ptr<xml::Node>& parent) const noexcept;

   /**
    * Set the feature of a service that must be initialized before this service.
    * \param feature Service that must be initialized before this service.
    * \warning It will not have any effect it this service has been initialized before calling this method.
    */
   void require(const Feature::_v feature) throw(basis::RuntimeException) { require(feature, WhateverImplementation); }

   /**
    * Set the feature of a service that must be initialized before this service.
    * \param feature Service that must be initialized before this service.
    * \warning It will not have any effect it this service has been initialized before calling this method.
    */
   void require(const Feature::_v feature, const std::string& implementation) throw(basis::RuntimeException);

protected:
   /**
    * Constructor.
    * \param app Application owner of this service.
    * \param className Logical name of this service.
    */
   Service(Application& app, const Feature::_v feature, const std::string& implementation) :
      Runnable(std::string(Feature::asString(feature)).append(":").append(implementation)),
      a_app(app),
      m_feature(feature),
      m_implementation(implementation),
      effectiveRunning(0)
   {
   }

   /**
    * This method should be implemented to specialize the initialization of this instance.
    * This method will be called from coffee::app::Application::start method.
    */
   void initialize() throw(basis::RuntimeException);

private:
   struct Requirement {
      const Feature::_v m_feature;
      const std::string m_implementation;
      std::shared_ptr<Service> m_selection;

      Requirement(const Feature::_v feature, const std::string& implementation) :
         m_feature(feature),
         m_implementation(implementation)
      {}

      std::string asString() const noexcept ;
   };

   typedef std::vector<Requirement> Requirements;

   Application& a_app;
   const Feature::_v m_feature;
   const std::string m_implementation;
   Requirements m_requirements;
   basis::Semaphore effectiveRunning;

   Service(const Service& other);

   virtual void do_initialize() throw(basis::RuntimeException) = 0;

   friend class Application;
};


}
}

#endif
