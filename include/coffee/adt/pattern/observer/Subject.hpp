#ifndef _coffee_adt_pattern_observer_Subject_hpp
#define _coffee_adt_pattern_observer_Subject_hpp

#include <unordered_map>
#include <set>
#include <memory>

#include <coffee/adt/RuntimeException.hpp>
#include <coffee/adt/NamedObject.hpp>

#include <coffee/adt/pattern/observer/Event.hpp>

namespace coffee {

namespace adt {

namespace pattern {

namespace observer {

class Observer;

class Subject : public NamedObject {
public:
   static const Event AllEvents;

   virtual ~Subject();

   void subscribeObserver(std::shared_ptr<Observer>& observer, const Event& event = AllEvents) throw(RuntimeException);
   void subscribeObserver(std::shared_ptr<Observer>& observer, const Event::Id eventId) throw(RuntimeException) {
      subscribeObserver(observer, lookupEvent(eventId));
   }
   bool unsubscribeObserver(const std::string& observerName) noexcept;

   size_t countObservers() const noexcept { return m_observers.size(); }

   virtual StreamString asString() const noexcept {
      StreamString result("pattern::observer::Subject {");
      return result << NamedObject::asString() << "}";
   }

protected:
   explicit Subject(const std::string& name) : NamedObject(name) {;}

   Event registerEvent(const Event::Id id) throw(RuntimeException);
   Event lookupEvent(const Event::Id id) const throw(RuntimeException);

   void notify(const Event::Id eventId) throw(RuntimeException) {
      notify(lookupEvent(eventId));
   }
   void notify(const Event& event = AllEvents) noexcept;

   typedef std::pair<std::shared_ptr<Observer>, Event::BitMask> Subscription;
   typedef std::unordered_map<std::string, Subscription> Observers;
   typedef Observers::iterator observer_iterator;
   Observers m_observers;

   observer_iterator observer_begin() noexcept { return m_observers.begin(); }
   observer_iterator observer_end() noexcept { return m_observers.end(); }
   static std::shared_ptr<Observer>& get_observer(observer_iterator ii) { return ii->second.first; }
   static Event::BitMask bitmask(observer_iterator ii) { return ii->second.second; }

   typedef std::set <Event> Events;
   typedef Events::iterator event_iterator;
   typedef Events::const_iterator const_event_iterator;
   Events m_events;

   event_iterator event_begin() noexcept { return m_events.begin(); }
   event_iterator event_end() noexcept { return m_events.end(); }
   const_event_iterator event_end() const noexcept { return m_events.end(); }
   static const Event& event(event_iterator ii) { return *ii; }
};

}
}
}
}

#endif
