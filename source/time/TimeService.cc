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

#include <coffee/basis/AsString.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TraceMethod.hpp>

#include <coffee/time/TimeService.hpp>
#include <coffee/time/SCCS.hpp>

#include <coffee/app/Application.hpp>

using namespace coffee;

using std::chrono::microseconds;
using std::chrono::milliseconds;

//static
const std::string time::TimeService::Implementation("native");

//static
std::shared_ptr<time::TimeService> time::TimeService::instantiate(app::Application& application, const milliseconds& maxTime, const milliseconds& resolution)
   throw(basis::RuntimeException)
{
   std::shared_ptr<TimeService> result(new TimeService(application, maxTime, resolution));
   application.attach(result);
   return result;
}

time::TimeService::TimeService(app::Application& application, const milliseconds& _maxTime, const milliseconds& _resolution) :
   app::Service(application, app::Feature::Timing, Implementation),
   basis::pattern::observer::Subject("TimeService"),
   maxTime(_maxTime),
   resolution(_resolution),
   maxQuantum(calculeMaxQuantum(_maxTime, _resolution)),
   currentQuantum(0)
{
   time::SCCS::activate();

   timeTable = new Quantum[maxQuantum];
   events.reserve(32);
}

time::TimeService::~TimeService()
{
   delete [] timeTable;
}

//static
int time::TimeService::calculeMaxQuantum(const milliseconds& maxTime, const milliseconds& resolution)
   noexcept
{
   int result = maxTime.count() / resolution.count();

   while ((result * resolution) < maxTime)
      ++ result;

   return result;
}

void time::TimeService::do_initialize()
   throw(basis::RuntimeException)
{
   if (maxTime <= resolution) {
      COFFEE_THROW_EXCEPTION("Resolution must be lesser than " << maxTime);
   }

   std::unique_lock<std::mutex> guard(mutex);

   producer = std::thread(produce, std::ref(*this));
   consumer = std::thread(consume, std::ref(*this));
}

void time::TimeService::do_stop()
   throw(basis::RuntimeException)
{
   LOG_THIS_METHOD();

   statusStopped();
   ticks.clear();

   if (!events.empty()) {
       LOG_WARN("There were " << events.size() << " events on air");
    }

   events.clear();
   condition.notify_all();
   producer.join();
   consumer.join();
}

void time::TimeService::activate(std::shared_ptr<TimeEvent> timeEvent)
   throw(basis::RuntimeException)
{
   if (isStopped()) {
      COFFEE_THROW_EXCEPTION(asString() << " can not accept new activation");
   }

   if (!timeEvent) {
      COFFEE_THROW_EXCEPTION("Time Event must be created by using TimeEvent::instantiate");
   }

   const milliseconds& timeout = timeEvent->getTimeout();

   if (timeout.count() == 0) {
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
   const milliseconds& timeout = timeEvent->getTimeout();

   const int indexQuantum = (currentQuantum + timeout / resolution) % maxQuantum;

   Quantum* target = (indexQuantum == currentQuantum) ? &temporaryQuantum: &timeTable[indexQuantum];
   target->push_front(timeEvent);
   Location location(target, target->begin());
   events[timeEvent->getId()] = location;

   auto now = std::chrono::high_resolution_clock::now();
   timeEvent->initTime = TimeService::now();
   timeEvent->endTime = milliseconds::zero();

   LOG_DEBUG("Now=" << timeEvent->initTime << " | CurrentQuantum=" << currentQuantum << " | IndexQuantum=" << indexQuantum << " | " << timeEvent->asString());
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

basis::StreamString time::TimeService::asString() const
   noexcept
{
   basis::StreamString result("time.TimeService{");
   result << Service::asString();
   result << " | MaxTime=" << maxTime;
   result << " | Resolution=" << resolution;
   result << " | MaxQuantum=" << maxQuantum;
   result << " | #TimeEvents=" << events.size();
   return result << "}";
}

//static
void time::TimeService::produce(time::TimeService& timeService)
   noexcept
{
   microseconds timeToWait(timeService.resolution);

   int tickCounter = 0;

   LOG_DEBUG("TimeToWait=" << timeToWait);

   timeService.notifyEffectiveRunning();

   const microseconds usResolution(timeService.resolution);

   while (timeService.isRunning()) {
      auto expectedTime = std::chrono::high_resolution_clock::now() + usResolution;

      usleep(timeToWait.count());

      if (true) {
         std::unique_lock <std::mutex> guard(timeService.mutex);
         timeService.ticks.push_back(++ tickCounter);
         timeService.condition.notify_one();
      }
      auto deviation = expectedTime - std::chrono::high_resolution_clock::now();
      timeToWait = timeToWait + std::chrono::duration_cast<microseconds>(deviation);
      LOG_LOCAL7("TimeToWait=" << timeToWait);
   }
}

//static
void time::TimeService::consume(TimeService& timeService)
   noexcept
{
   std::unique_lock<std::mutex> guard(timeService.mutex);

   while (timeService.isRunning()) {
      timeService.condition.wait(guard);

      while (!timeService.ticks.empty()) {
         timeService.ticks.pop_front();

         Quantum& timedout = timeService.timeTable[timeService.currentQuantum];

         const milliseconds now = TimeService::now();

         LOG_LOCAL7("Now=" << now << " | CurrentQuantum=" << timeService.currentQuantum);

         for (quantum_iterator ii = timedout.begin(), maxii = timedout.end(); ii != maxii; ++ ii) {
            std::shared_ptr<TimeEvent> timeEvent = *ii;
            timeEvent->endTime = now;
            LOG_DEBUG("CurrentQuantum=" << timeService.currentQuantum << " | " << timeEvent->asString());
            timeService.events.erase(timeEvent->getId());
            timeService.notify(*timeEvent);

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
}

milliseconds time::TimeService::now()
   noexcept
{
   // See https://stackoverflow.com/questions/9089842/c-chrono-system-time-in-milliseconds-time-operations
   return std::chrono::duration_cast<milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
}

std::chrono::seconds time::TimeService::toSeconds(const std::chrono::milliseconds& millisecond)
   noexcept
{
   return std::chrono::duration_cast<std::chrono::seconds>(millisecond);
}
