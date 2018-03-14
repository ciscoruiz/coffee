// MIT License
// 
// Copyright(c) 2018 Francisco Ruiz(francisco.ruiz.rayo@gmail.com)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
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

#ifndef _coffee_adt_pattern_observer_Event_hpp
#define _coffee_adt_pattern_observer_Event_hpp

#include <coffee/adt/StreamString.hpp>

namespace coffee {

namespace adt {

namespace pattern {

namespace observer {

class Subject;

/**
 * Define the different events that a Subject can generate.
 */
class Event {
public:
   typedef int Id;

   /**
    * Constructor.
    */
   Event() : m_id(NullId) {;}

   /**
    * Constructor.
    * \param id Event id. It must be unique for the associated Subject.
    */
   explicit Event(const Id id) : m_id(id) {;}

   /**
    * Copy constructor.
    */
   Event(const Event& other) : m_id(other.m_id) {;}

   /**
    * Destructor
    */
   virtual ~Event() {;}

   /**
    * \return The Id of this instance.
    */
   Id getId() const noexcept { return m_id; }

   /**
    * \return \b true if the IDs of both events are the same or \b false otherwise
    */
   bool operator==(const Event& other) const noexcept { return m_id == other.m_id; }

   /**
    * operator assignament.
    */
   Event& operator=(const Event& other) {
      m_id = other.m_id;
      return *this;
   }

   /**
    * \return Summarize information of the instance
    */
   virtual adt::StreamString asString() const noexcept {
      adt::StreamString result("pattern.observer.Event {");
      result << "Id=" << m_id;
      return result << "}";
   }
private:
   static const Id NullId = -1;

   Id m_id;

   friend class Subject;
};

}
}
}
}

#endif /* EVENT_HPP_ */
