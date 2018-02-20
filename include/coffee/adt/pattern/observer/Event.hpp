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

#ifndef _coffee_adt_pattern_observer_Event_hpp
#define _coffee_adt_pattern_observer_Event_hpp

namespace coffee {

namespace adt {

namespace pattern {

namespace observer {

class Subject;

class Event {
public:
   typedef int Id;
   typedef unsigned int BitMask;

   static const Id NullId = -1;

   Event () : m_id (NullId), m_bitMask (0) {;}
   Event (const Id id, const BitMask bitMask) : m_id (id), m_bitMask (bitMask) {;}
   Event (const Event& other) : m_id (other.m_id), m_bitMask (other.m_bitMask) {;}

   Id getId () const noexcept { return m_id; }
   BitMask getBitMask () const noexcept { return m_bitMask; }

   bool operator== (const Event& other) const noexcept { return m_id == other.m_id; }

   Event& operator= (const Event& other) {
      m_id = other.m_id;
      m_bitMask = other.m_bitMask;
      return *this;
   }

private:
   Id m_id;
   BitMask m_bitMask;

   friend class Subject;
};

inline Event operator+ (const Event& left, const Event& right)
   noexcept
{
   return Event (Event::NullId, left.getBitMask () | right.getBitMask ());
}

inline bool operator < (const Event& right, const Event& left)
   noexcept
{
   return right.getId () < left.getId ();
}

}
}
}
}

#endif /* EVENT_HPP_ */
