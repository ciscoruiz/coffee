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

#include <unistd.h>

#include <coffee/adt/Microsecond.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TraceMethod.hpp>

#include <coffee/time/TimeService.hpp>
#include <coffee/time/SCCS.hpp>

#include <coffee/app/Application.hpp>

using namespace coffee;

//static
std::shared_ptr<time::TimeService> time::TimeService::instantiate(app::Application& application, const adt::Millisecond& maxTime, const adt::Millisecond& resolution)
   throw(adt::RuntimeException)
{
   std::shared_ptr<TimeService> result(new TimeService(application, maxTime, resolution));
   application.attach(result);
   return result;
}

time::TimeService::TimeService(app::Application& application, const adt::Millisecond& _maxTime, const adt::Millisecond& _resolution) :
   app::Service(application, "TimeService"),
   adt::pattern::observer::Subject("TimeService"),
   maxTime(_maxTime),
   resolution(_resolution),
   maxQuantum(calculeMaxQuantum(_maxTime, _resolution)),
   currentQuantum(0),
   producerIsWorking(0)
{
   time::SCCS::activate();

   timeTable = new Quantum[maxQuantum];
   events.reserve(32);
}

time::TimeService::~TimeService()
{
   if (!events.empty()) {
      LOG_WARN("There were " << events.size() << " events on air");
   }

   delete [] timeTable;
}

//static
int time::TimeService::calculeMaxQuantum(const adt::Millisecond& maxTime, const adt::Millisecond& resolution)
   noexcept
{
   const adt::Millisecond::type_t _maxTime = maxTime.getValue();
   const adt::Millisecond::type_t _resolution= resolution.getValue();

   int result = _maxTime / _resolution;

   while ((result * _resolution) < _maxTime)
      ++ result;

   return result;
}

void time::TimeService::do_initialize()
   throw(adt::RuntimeException)
{
   if (maxTime <= resolution) {
      COFFEE_THROW_EXCEPTION("Resolution must be lesser than " << maxTime);
   }

   producer = std::thread(produce, std::ref(*this));
   consumer = std::thread(consume, std::ref(*this));
}

void time::TimeService::do_stop()
   throw(adt::RuntimeException)
{
   statusStopped();
   condition.notify_all();
   producer.join();
   consumer.join();
}

void time::TimeService::activate(std::shared_ptr<TimeEvent> timeEvent)
   throw(adt::RuntimeException)
{
   if (isStopped()) {
      COFFEE_THROW_EXCEPTION(asString() << " can not accept new activation");
   }

   if (!timeEvent) {
      COFFEE_THROW_EXCEPTION("Time Event must be created by using TimeEvent::instantiate");
   }

   const adt::Millisecond& timeout = timeEvent->getTimeout();

   if (timeout == 0) {
      COFFEE_THROW_EXCEPTION("Event duration should be greater that 0");
   }

   if (timeout > maxTime || timeout < resolution) {
      COFFEE_THROW_EXCEPTION("Timeout=" << timeout << " is out range for " << asString());
   }

   std::unique_lock<std::mutex> guard(mutex);

   if (events.find(timeEvent->getId()) != events.end()) {
      COFFEE_THROW_EXCEPTION(timeEvent->getId () << " already activated");
   }

   store(timeEvent, guard);
}

void time::TimeService::store(std::shared_ptr<TimeEvent> timeEvent, std::unique_lock<std::mutex>& guard)
   noexcept
{
   const adt::Millisecond& timeout = timeEvent->getTimeout();

   const int indexQuantum = (currentQuantum + timeout.getValue() / resolution.getValue()) % maxQuantum;

   Quantum* target = (indexQuantum == currentQuantum) ? &temporaryQuantum: &timeTable[indexQuantum];
   target->push_front(timeEvent);
   Location location(target, target->begin());
   events[timeEvent->getId()] = location;
   timeEvent->initTime = adt::Millisecond::getTime();

   LOG_DEBUG("Now=" << adt::Millisecond::getTime().asString() << " | CurrentQuantum=" << currentQuantum << " | IndexQuantum=" << indexQuantum << " | " << timeEvent->asString());
}

bool time::TimeService::cancel(std::shared_ptr<TimeEvent> timeEvent)
   noexcept
{
   if (!timeEvent)
      return false;

   std::unique_lock<std::mutex> guard(mutex);

   auto ii = events.find(timeEvent->getId());

   if (ii == events.end())
      return false;

   ii->second.first->erase(ii->second.second);
   events.erase(ii);
   return true;
}

bool time::TimeService::empty()
   noexcept
{
   std::unique_lock<std::mutex> guard(mutex);
   return events.empty();
}

adt::StreamString time::TimeService::asString() const
   noexcept
{
   adt::StreamString result("time.TimeService{");
   result << Service::asString();
   result << " | MaxTime=" << maxTime.asString();
   result << " | Resolution=" << resolution.asString();
   result << " | MaxQuantum=" << maxQuantum;
   result << " | #TimeEvents=" << events.size();
   return result << "}";
}

//static
void time::TimeService::produce(time::TimeService& timeService)
   noexcept
{
   adt::Microsecond timeToWait(timeService.resolution);
   adt::Millisecond now = adt::Millisecond::getTime();
   int tickCounter = 0;

   LOG_DEBUG("Now=" << now.asString() <<" | TimeToWait=" << timeService.resolution.asString());

   timeService.producerIsWorking.signal();

   const adt::Microsecond usResolution (timeService.resolution);

   while (timeService.isRunning()) {
      const adt::Microsecond nextTime = adt::Microsecond::getTime() + usResolution;
      usleep(timeToWait);
      if (true) {
         std::unique_lock <std::mutex> guard(timeService.mutex);
         timeService.ticks.push_back(++ tickCounter);
         timeService.condition.notify_one();
      }
      const adt::Microsecond deviation = nextTime - adt::Microsecond::getTime();
      timeToWait = timeToWait + deviation;
   }
}

//static
void time::TimeService::consume(TimeService& timeService)
   noexcept
{
   std::unique_lock<std::mutex> guard(timeService.mutex);
   adt::Millisecond now = adt::Millisecond::getTime();

   LOG_DEBUG("Now=" << now.asString());

   while (timeService.isRunning()) {
      timeService.condition.wait(guard);

      if (timeService.ticks.empty())
         continue;

      timeService.ticks.pop_front();

      Quantum& timedout = timeService.timeTable[timeService.currentQuantum];

      now = adt::Millisecond::getTime();
      LOG_LOCAL7("Now=" << now.asString() << " | CurrentQuantum=" << timeService.currentQuantum);

      for (quantum_iterator ii = timedout.begin(), maxii = timedout.end(); ii != maxii; ++ ii) {
         std::shared_ptr<TimeEvent> timeEvent = *ii;
         timeEvent->endTime = now;
         LOG_LOCAL7(timeEvent->asString());
         timeService.notify(*timeEvent);
         timeService.events.erase(timeEvent->getId());

         if (timeEvent->isPeriodical())
            timeService.store(timeEvent, guard);
      }

      timedout.clear();

      LOG_LOCAL7("CurrentQuantum=" << timeService.currentQuantum << " has been processed");

      if (!timeService.temporaryQuantum.empty()) {
         timedout.splice(timedout.end(), timeService.temporaryQuantum);
      }

      if (++ timeService.currentQuantum == timeService.maxQuantum) {
         timeService.currentQuantum = 0;
      }
   }
}

