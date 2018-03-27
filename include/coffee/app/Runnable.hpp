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

#ifndef _coffee_app_Runnable_h
#define _coffee_app_Runnable_h

#include <memory>

#include <coffee/basis/RuntimeException.hpp>
#include <coffee/basis/NamedObject.hpp>
#include <coffee/basis/StreamString.hpp>

namespace coffee {

namespace xml {
class Node;
}

namespace app {

/**
 * Generic class for other classes which has to done an unique specific operation during a
 * undefined period of time.
 */
class Runnable : public basis::NamedObject {
   struct StatusFlags { enum _v { Stopped = 0, Starting = 1, Running = 2, StoppedWithError = 4 }; };

public:
   /**
    * Destructor.
    */
   virtual ~Runnable() { ;}

   /**
    * \return \b true if this instance is stopped or \b false otherwise.
    */
   bool isStopped() const noexcept { return m_statusFlags == StatusFlags::Stopped || m_statusFlags == StatusFlags::StoppedWithError; }

   /**
    * \return \b true if this instance had and error during stop processing or \b false otherwise.
    */
   bool isStoppedWithError() const noexcept { return(m_statusFlags & StatusFlags::StoppedWithError) != 0; }

   /**
    * \return \b true if this instance is starting its execution or \b false otherwise.
    */
   bool isStarting() const noexcept { return(m_statusFlags & StatusFlags::Starting) != 0; }

   /**
    * \return \b true if this instance is running or \b false otherwise.
    */
   bool isRunning() const noexcept { return(m_statusFlags & StatusFlags::Running) != 0; }

   /**
    * Process the request to stop, it will act over internal members and it will call virtual pure method do_stop.
    * \return \b true if this instance was in an state which could accept the stop or \b false otherwise.
    */
   bool stop() throw(basis::RuntimeException);

   /**
    * \return Summarize information of the instance as basis::StreamString
    */
   virtual basis::StreamString asString() const noexcept;

   /**
    * \return Summarize information of the instance as coffee::xml::Node
    */
   virtual std::shared_ptr<xml::Node> asXML(std::shared_ptr<xml::Node>& parent) const noexcept;

protected:
   /**
     Constructor.
     @param name Logic name.
   */
   explicit Runnable(const std::string& name) : basis::NamedObject(name), m_statusFlags(StatusFlags::Stopped) {;}

   /**
    * Constructor .
    */
   Runnable() : basis::NamedObject(""), m_statusFlags(StatusFlags::Stopped) {;}

   /**
    * Activates the flag received as parameter.
    */
   void activate(const StatusFlags::_v statusFlag) noexcept { m_statusFlags |= statusFlag; }

   /**
    * Deactivates the flag received as parameter.
    */
   void deactivate(const StatusFlags::_v statusFlag) noexcept { m_statusFlags &= ~statusFlag; }

   /**
    * Set flags to indicate starting of this instance has began.
    */
   void statusStarting() noexcept { m_statusFlags = StatusFlags::Starting; }

   /**
    * Set flags to indicate this instance is running.
    */
   void statusRunning() noexcept {
      deactivate(StatusFlags::Starting);
      activate(StatusFlags::Running);
   }

   /**
    * Set flags to indicate this instance is stopped.
    */
   void statusStopped() noexcept { m_statusFlags = StatusFlags::Stopped; }

   /**
    * Pure virtual method to fine tuning operations to done in case of receive a
    * request for stop.
    */
   virtual void do_stop() throw(basis::RuntimeException) = 0;

private:
   int m_statusFlags;

   std::string flagsAsString() const noexcept;
};

}
}


#endif

