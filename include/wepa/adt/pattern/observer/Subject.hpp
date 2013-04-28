// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2013 Francisco Ruiz Rayo
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#ifndef _wepa_adt_pattern_observer_Subject_hpp
#define _wepa_adt_pattern_observer_Subject_hpp

#include <vector>
#include <set>

#include <wepa/adt/RuntimeException.hpp>
#include <wepa/adt/NamedObject.hpp>

#include <wepa/adt/pattern/observer/Event.hpp>

namespace wepa {

namespace adt {

namespace pattern {

namespace observer {

class Observer;

class Subject : public NamedObject {
   static const Event nullEvent;

public:
   virtual ~Subject ();

   void subscribeObserver (Observer* observer, const Event& event = nullEvent) throw (RuntimeException);
   void subscribeObserver (Observer* observer, const Event::Id eventId) throw (RuntimeException) {
      subscribeObserver(observer, lookupEvent(eventId));
   }
   bool unsubscribeObserver (Observer* observer) throw ();

   virtual StreamString asString () const throw () {
      StreamString result ("pattern::observer::Subject {");
      return result << NamedObject::asString() << "}";
   }

protected:
   Subject (const std::string& name) : NamedObject(name) {;}

   Event registerEvent (const Event::Id id) throw (RuntimeException);
   Event lookupEvent (const Event::Id id) throw (RuntimeException);

   void notify (const Event::Id eventId) throw (RuntimeException) {
      notify (lookupEvent(eventId));
   }
   void notify (const Event& event = nullEvent) throw (RuntimeException);

   typedef std::pair <Observer*, Event::BitMask> Subscription;
   typedef std::vector <Subscription> Observers;
   typedef Observers::iterator observer_iterator;
   Observers m_observers;

   observer_iterator observer_begin () throw () { return m_observers.begin (); }
   observer_iterator observer_end () throw () { return m_observers.end (); }
   static Observer* get_observer (observer_iterator ii) { return ii->first; }
   static Event::BitMask bitmask (observer_iterator ii) { return ii->second; }

   typedef std::set <Event> Events;
   typedef Events::iterator event_iterator;
   Events m_events;

   event_iterator event_begin () throw () { return m_events.begin (); }
   event_iterator event_end () throw () { return m_events.end (); }
   static const Event& event (event_iterator ii) { return *ii; }
};

}
}
}
}

#endif
