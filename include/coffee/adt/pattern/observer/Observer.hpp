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

#ifndef _coffee_adt_pattern_observer_Observer_h
#define _coffee_adt_pattern_observer_Observer_h

#include <string>
#include <memory>

#include <coffee/adt/RuntimeException.hpp>
#include <coffee/adt/NamedObject.hpp>

namespace coffee {

namespace adt {

namespace pattern {

namespace observer {

class Event;
class Subject;

/**
 * The responsibility of observers is to register (and unregister) themselves on a subject (to get notified of state changes)
 * and to update their state (synchronize their state with subject's state) when they are notified.
 */
class Observer : public NamedObject {
public:
   /**
    * Destructor.
    */
   virtual ~Observer() {;}

   /**
    * \return \b true if this observer is attached to some subject or \b false otherwise.
    */
   bool isSubscribed() const noexcept { return m_isSubscribed; }

protected:
   /**
    * Constructor
    * \param name Logical name.
    */
   explicit Observer(const std::string& name) : NamedObject(name), m_isSubscribed(false) {;}

   /**
    * Receive notification to be aware that was attached to the received subject.
    * \param subject that changes its state.
    */
   virtual void attached(const Subject& subject) noexcept = 0 ;

   /**
    * Receive notification about change in the subject.
    * \param subject that changes its state.
    * \param event Event information. Depending of his event the observer could read different members of the subject.
    */
   virtual void update(const Subject& subject, const Event& event) noexcept = 0 ;

   /**
    * Receive notification to be aware that was detached from the received subject.
    * \param subject that changes its state.
    */
   virtual void detached(const Subject& subject) noexcept {;}

private:
   bool m_isSubscribed;

   void isSubscribed(const bool value) noexcept { m_isSubscribed = value; }

   friend class Subject;
};

}
}
}
}


#endif

