// WEPA - Write Excellent Professional Applications
//
//(c) Copyright 2013 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/wepa
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//

#include <wepa/adt/pattern/observer/Subject.hpp>
#include <wepa/adt/pattern/observer/Observer.hpp>
#include <wepa/logger/Logger.hpp>

using namespace wepa::adt::pattern;

//static
const observer::Event observer::Subject::AllEvents(-1, 0xffffffff);

observer::Subject::~Subject()
{
   for (auto ii = observer_begin(); ii != observer_end(); ++ ii) {
      get_observer(ii)->isSubscribed(false);
   }
   m_observers.clear();
}

void observer::Subject::subscribeObserver(std::shared_ptr<Observer>& observer, const Event& event)
   throw(RuntimeException)
{
   const std::string& observerName = observer->getName();

   if (m_observers.find(observerName) != observer_end()) {
      LOG_WARN(observer->getName() << " already connected to " << getName());
      return;
   }

   if (observer->isSubscribed()) {
      WEPA_THROW_EXCEPTION(observer->asString() << " already subscribed");
   }

   m_observers.insert(std::make_pair(observerName, Subscription(observer, event.getBitMask())));
   observer->isSubscribed(true);

   // Put status in common
   observer->update(*this, event);
}

bool observer::Subject::unsubscribeObserver(const std::string& observerName)
   noexcept
{
   const auto ii = m_observers.find(observerName);

   if (ii == m_observers.end())
      return false;

   m_observers.erase(ii);

   return true;
}

observer::Event observer::Subject::registerEvent(const Event::Id id)
   throw(RuntimeException)
{
   if (m_events.size() >= sizeof(Event::BitMask) * 8) {
      WEPA_THROW_EXCEPTION("Too many events for " << asString());
   }

   Event search(id, 0);
   event_iterator ii;

   if ((ii = m_events.find(search)) != event_end())
      return event(ii);

   Event newEvent(id, 1 << m_events.size());

   m_events.insert(newEvent);

   return newEvent;
}

observer::Event observer::Subject::lookupEvent(const Event::Id id)
   throw(RuntimeException)
{
   Event search(id, 0);
   event_iterator ii;

   if ((ii = m_events.find(search)) != event_end())
      return event(ii);

   WEPA_THROW_EXCEPTION("EventId=" << id << " was not registered");

   // To avoid warning about no return value
   return search;
}

void observer::Subject::notify(const Event& event)
   noexcept
{
   for(auto ii = observer_begin(), maxii = observer_end(); ii != maxii; ++ ii) {
      Event::BitMask bitmask = Subject::bitmask(ii);

      if ((bitmask & event.getBitMask()) != 0)
         observer::Subject::get_observer(ii)->update(*this, event);
   }
}
