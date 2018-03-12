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

#include <coffee/adt/pattern/observer/Event.hpp>
#include <coffee/adt/Millisecond.hpp>

namespace coffee {
namespace time {

class TimeEvent : public adt::pattern::observer::Event {
public:
   virtual ~TimeEvent() {;}

   const adt::Millisecond& getTimeout() const noexcept { return timeout; }

   virtual bool isPeriodical() const noexcept = 0;

   /**
    * \return Summarize information of the subject
    */
   virtual adt::StreamString asString() const noexcept {
      adt::StreamString result("time.TimeEvent {");
      result << Event::asString();
      result << " | Timeout=" << timeout.asString();
      return result << "}";
   }

protected:
   TimeEvent(const Id id, const adt::Millisecond& _timeout) : adt::pattern::observer::Event(id), timeout(_timeout) {;}

private:
   const adt::Millisecond timeout;

};

}
}

#endif /* _coffee_time_TimeEvent_hpp_ */
