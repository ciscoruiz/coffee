#include <coffee/adt/pattern/observer/Subject.hpp>
#include <coffee/adt/pattern/observer/Observer.hpp>
#include <coffee/logger/Logger.hpp>

using namespace coffee::adt::pattern;

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
      return;
   }

   if (observer->isSubscribed()) {
      COFFEE_THROW_EXCEPTION(observer->asString() << " already subscribed");
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
      COFFEE_THROW_EXCEPTION("Too many events for " << asString());
   }

   Event search(id, 0);
   event_iterator ii;

   if ((ii = m_events.find(search)) != event_end())
      return event(ii);

   Event newEvent(id, 1 << m_events.size());

   m_events.insert(newEvent);

   return newEvent;
}

observer::Event observer::Subject::lookupEvent(const Event::Id id) const
   throw(RuntimeException)
{
   Event search(id, 0);
   const_event_iterator ii;

   if ((ii = m_events.find(search)) != event_end())
      return event(ii);

   COFFEE_THROW_EXCEPTION("EventId=" << id << " was not registered");

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
