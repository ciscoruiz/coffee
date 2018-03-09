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

#ifndef _coffee_time_TimeService_hpp_
#define _coffee_time_TimeService_hpp_

#include <list>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <thread>

#include <coffee/app/Service.hpp>
#include <coffee/adt/pattern/observer/Subject.hpp>
#include <coffee/time/TimeEvent.hpp>

namespace coffee {

namespace time {

class TimeEvent;

class TimeService : public app::Service, public adt::pattern::observer::Subject {
public:
   static std::shared_ptr<TimeService> instantiate(app::Application& application, const adt::Millisecond& maxTime, const adt::Millisecond& resolution)
      throw(adt::RuntimeException);

   ~TimeService();

   void activate(std::shared_ptr<TimeEvent> timeEvent) throw(adt::RuntimeException);
   bool cancel(std::shared_ptr<TimeEvent> timeEvent) noexcept;

   adt::StreamString asString() const noexcept;

private:
   typedef std::list<std::shared_ptr<TimeEvent> > Quantum;
   typedef Quantum::iterator quantum_iterator;

   typedef std::pair<Quantum*, quantum_iterator> Location;
   typedef std::unordered_map<TimeEvent::Id, Location> Events;

   const adt::Millisecond maxTime;
   const adt::Millisecond resolution;
   const int maxQuantum;
   int currentQuantum;
   Quantum* timeTable;
   Events events;
   Quantum temporaryQuantum;

   std::mutex mutex;
   std::condition_variable condition;
   std::list<int> ticks;
   std::thread consumer;
   std::thread producer;

   TimeService(app::Application& application, const adt::Millisecond& maxTime, const adt::Millisecond& resolution);
   static int calculeMaxQuantum(const adt::Millisecond& maxTime, const adt::Millisecond& resolution) noexcept;
   static void consume(TimeService& timeService) noexcept;
   static void produce(TimeService& timeService) noexcept;

   void timeout(std::unique_lock<std::mutex>& guard) throw(adt::RuntimeException);
   void store(std::shared_ptr<TimeEvent> timeEvent, std::unique_lock<std::mutex>& guard) noexcept;

   void do_initialize() throw(adt::RuntimeException) ;
   void do_stop() throw(adt::RuntimeException) ;
};

}
}


#endif /* _coffee_time_TimeService_hpp_ */
