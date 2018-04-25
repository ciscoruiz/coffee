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
#include <chrono>

#include <coffee/app/Service.hpp>
#include <coffee/basis/pattern/observer/Subject.hpp>
#include <coffee/time/TimeEvent.hpp>

namespace coffee {

namespace time {

class TimeEvent;

/**
 * Service for managing asynchronous time events.
 */
class TimeService : public app::Service, public basis::pattern::observer::Subject {
public:
   static const std::string Implementation;

   /**
    * Fast instantiation for this service.
    */
   static std::shared_ptr<TimeService> instantiate(app::Application& application, const std::chrono::milliseconds& maxTime, const std::chrono::milliseconds& resolution)
      throw(basis::RuntimeException);

   /**
    * Destructor.
    */
   ~TimeService();

   /**
    * \return This current time expressed in milliseconds.
    */
   static std::chrono::milliseconds now() noexcept;

   /**
    * \return Converts milliseconds to seconds.
    */
   static std::chrono::seconds toSeconds(const std::chrono::milliseconds& millisecond) noexcept;

   /**
    * Activate the time event. Once the duration of this event has been reach the associated
    * observer will be notified.
    * \include test/time/Timer_test.cc
    */
   void activate(std::shared_ptr<TimeEvent> timeEvent) throw(basis::RuntimeException);

   /*
    * Cancel the time event.
    * \return \b true if the time event can be cancelled or \b false otherwise.
    */
   bool cancel(std::shared_ptr<TimeEvent> timeEvent) noexcept;

   /**
    * \return \b true if there is not any event activated or \b false otherwise.
    */
   bool empty() const noexcept { return events.empty(); }

   /**
    * \return Numbers of activated time events.
    */
   size_t size() const noexcept { return events.size(); }

   /**
    * \return Summarize information of the instance
    */
   basis::StreamString asString() const noexcept;

private:
   typedef std::list<std::shared_ptr<TimeEvent> > Quantum;
   typedef Quantum::iterator quantum_iterator;

   typedef std::pair<Quantum*, quantum_iterator> Location;
   typedef std::unordered_map<TimeEvent::Id, Location> Events;

   const std::chrono::milliseconds maxTime;
   const std::chrono::milliseconds resolution;
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

   TimeService(app::Application& application, const std::chrono::milliseconds& maxTime, const std::chrono::milliseconds& resolution);
   static int calculeMaxQuantum(const std::chrono::milliseconds& maxTime, const std::chrono::milliseconds& resolution) noexcept;
   static void consume(TimeService& timeService) noexcept;
   static void produce(TimeService& timeService) noexcept;

   void timeout(std::unique_lock<std::mutex>& guard) throw(basis::RuntimeException);
   void store(std::shared_ptr<TimeEvent> timeEvent, std::unique_lock<std::mutex>& guard) noexcept;

   void do_initialize() throw(basis::RuntimeException) ;
   void do_stop() throw(basis::RuntimeException) ;
};

}
}


#endif /* _coffee_time_TimeService_hpp_ */
