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
#include <memory>

#include <coffee/adt/RuntimeException.hpp>
#include <coffee/adt/NamedObject.hpp>

namespace coffee {

namespace adt {

namespace pattern {

namespace observer {

class Observer;
class Event;

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
    * Destructor
    */
   virtual ~Subject();

   /**
    * Subscribe the observer to change on this subject.
    */
   void attach(std::shared_ptr<Observer> observer) throw(RuntimeException);

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
   virtual StreamString asString() const noexcept;

protected:
   typedef std::unordered_map<std::string, std::shared_ptr<Observer> > Observers;
   typedef Observers::iterator observer_iterator;

   /**
    * Constructor
    */
   explicit Subject(const std::string& name) : NamedObject(name) {;}

   /**
    * Notify that event has occurred over this subject.
    */
   void notify(const Event& event) noexcept;

   /**
    * \return service_iterator to the first attached observer.
    */
   observer_iterator observer_begin() noexcept { return m_observers.begin(); }

   /**
    * \return service_iterator to the last attached observer.
    */
   observer_iterator observer_end() noexcept { return m_observers.end(); }

   /**
    * \return the observer addressed by the observer_iterator.
    * \warning the value ii must be contained in [#observer_begin, #observer_end)
    */
   static std::shared_ptr<Observer>& observer(observer_iterator ii) { return ii->second; }

private:
   Observers m_observers;
};

}
}
}
}

#endif
