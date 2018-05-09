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

#include <coffee/time/TimeEvent.hpp>

using namespace coffee;

using std::chrono::milliseconds;

time::TimeEvent::TimeEvent(const Id id, const milliseconds& _timeout) :
   basis::pattern::observer::Event(id), timeout(_timeout),
   initTime(0),
   endTime(0)
{;}

milliseconds time::TimeEvent::getDuration() const
   throw(basis::RuntimeException)
{
   if (!isStarted() || !isFinished()) {
      COFFEE_THROW_EXCEPTION(asString() << " was not started or finished");
   }

   return endTime - initTime;
}

//virtual
basis::StreamString time::TimeEvent::asString() const
   noexcept
{
   basis::StreamString result("time.TimeEvent {");
   result << Event::asString();
   result << " | Timeout=" << timeout;

   if (isStarted() && isFinished()) {
      milliseconds ww(0);
      try {
         ww = getDuration();
      }
      catch(...) {
      }
      result << " | Duration=" << ww;
   }

   return result << "}";
}
