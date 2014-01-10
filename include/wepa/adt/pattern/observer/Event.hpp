// - Write Excellent Professional Applications
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
#ifndef _wepa_adt_pattern_observer_Event_hpp
#define _wepa_adt_pattern_observer_Event_hpp

namespace wepa {

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
