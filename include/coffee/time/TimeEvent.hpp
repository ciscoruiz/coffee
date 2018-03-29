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

#ifndef _coffee_time_TimeEvent_hpp_
#define _coffee_time_TimeEvent_hpp_

#include <memory>
#include <chrono>

#include <coffee/basis/RuntimeException.hpp>
#include <coffee/basis/pattern/observer/Event.hpp>

namespace coffee {
namespace time {

class TimeService;

/**
 * Class base for different types of time events.
 */
class TimeEvent : public basis::pattern::observer::Event {
public:
   /**
    * Destructor.
    */
   virtual ~TimeEvent() {;}

   /**
    * \return The timeout of this instance.
    */
   const std::chrono::milliseconds& getTimeout() const noexcept { return timeout; }

   /**
    * \return \b true if the event been started into the TimeService.
    */
   bool isStarted() const throw () { return initTime.count() != 0; }

   /**
    * \return \b true if the event has been finished into the TimeService.
    */
   bool isFinished() const throw () { return endTime.count() != 0; }

   /**
    * \return The final duration of this event.
    * \warning It should not be valid until #isFinished returns \b true
    */
   std::chrono::milliseconds getDuration() const throw(basis::RuntimeException);

   /**
    * \return \b true if the event will be re-scheduled once it has been finished or \b false otherwise.
    */
   virtual bool isPeriodical() const noexcept = 0;

   /**
    * \return Summarize information of the TimeEvent
    */
   virtual basis::StreamString asString() const noexcept;

protected:
   /**
    * Constructor.
    * \param id Unique identification for this event.
    * \param timeout Expected duration of this event once it has been activated.
    */
   TimeEvent(const Id id, const std::chrono::milliseconds& timeout);

private:
   const std::chrono::milliseconds timeout;
   std::chrono::milliseconds initTime;
   std::chrono::milliseconds endTime;

   friend class TimeService;
};

}
}

#endif /* _coffee_time_TimeEvent_hpp_ */
