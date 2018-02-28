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

/**
 * This is the \b Subject of Observer pattern.
 *
 * \see https://en.wikipedia.org/wiki/Observer_pattern
 *
 * The Observer pattern addresses the following problems:
 *  \li A one-to-many dependency between objects should be defined without making the objects tightly coupled.
 *  \li It should be ensured that when one object changes state an open-ended number of dependent objects are updated automatically.
 *  \li It should be possible that one object can notify an open-ended number of other objects.
 *
 *  \include PatternObserver_test.cc
 */
class Subject : public NamedObject {
public:
   /**
    * Instance for subscription to all events.
    */
   static const Event AllEvents;

   /**
    * Destructor
    */
   virtual ~Subject();

   /**
    * Subscribe the observer to change on this subject.
    */
   void attach(std::shared_ptr<Observer> observer, const Event& event = AllEvents) throw(RuntimeException);

   /**
    * Subscribe the observer to change on this subject.
    * \warning the \b eventId parameter must be previously registered by using #registerEvent
    */
   void attach(std::shared_ptr<Observer> observer, const Event::Id eventId) throw(RuntimeException) {
      attach(observer, lookupEvent(eventId));
   }

   /**
    * \return \b true if the observer was removed from the list of observers or \b false otherwise.
    */
   bool detach(const std::string& observerName) noexcept;

   /**
    * \return Numbers of subscribers associated to this subject.
    */
   size_t countObservers() const noexcept { return m_observers.size(); }

   /**
    * \return Summarize information of the subject
    */
   virtual StreamString asString() const noexcept {
      StreamString result("pattern::observer::Subject {");
      return result << NamedObject::asString() << "}";
   }

protected:
   /**
    * Constructor
    */
   explicit Subject(const std::string& name) : NamedObject(name) {;}

   /**
    * \return The event which will be identified by this id.
    */
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
