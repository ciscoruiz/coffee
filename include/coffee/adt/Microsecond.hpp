// COFFEE - COmpany eFFEEctive Platform
//
// (c) Copyright 2018 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/coffee
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
#ifndef _coffee_adt_Microsecond_h
#define _coffee_adt_Microsecond_h

#include <string>

#include <coffee/config/defines.hpp>

#include <coffee/adt/RuntimeException.hpp>

namespace coffee {

namespace adt {

class Millisecond;
class Second;

class Microsecond {
public:
   typedef Integer64 type_t;

   Microsecond() : m_value (0) {;}

   explicit Microsecond(const type_t value) : m_value (value) {;}

   Microsecond (const Microsecond& other) : m_value (other.m_value) {;}

   Microsecond (const Millisecond& other);

   Microsecond (const Second& other);

   operator type_t () const noexcept { return m_value; }

   Microsecond& operator= (const type_t other) noexcept { m_value = other; return *this; }

   Microsecond& operator= (const Microsecond& other) noexcept { m_value = other.m_value; return *this; }

   Microsecond& operator= (const Millisecond& other) noexcept;

   Microsecond& operator= (const Second& other) noexcept;

   bool operator== (const Microsecond& other) const noexcept { return m_value == other.m_value; }

   bool operator== (const Millisecond& other) const noexcept;

   bool operator== (const Second& other) const noexcept;

   bool operator!= (const Microsecond& other) const noexcept { return m_value != other.m_value; }

   bool operator!= (const Millisecond& other) const noexcept;

   bool operator!= (const Second& other) const noexcept;

   bool operator> (const Microsecond& other) const noexcept { return m_value > other.m_value; }

   bool operator> (const Millisecond& other) const noexcept;

   bool operator> (const Second& other) const noexcept;

   bool operator< (const Microsecond& other) const noexcept { return m_value < other.m_value; }

   bool operator< (const Millisecond& other) const noexcept;

   bool operator< (const Second& other) const noexcept;

   bool operator>= (const Microsecond& other) const noexcept { return m_value >= other.m_value; }

   bool operator>= (const Millisecond& other) const noexcept { return (operator==(other) == true) ? true: operator>(other); }

   bool operator>= (const Second& other) const noexcept { return (operator==(other) == true) ? true: operator>(other); }

   bool operator<= (const Microsecond& other) const noexcept { return m_value <= other.m_value; }

   bool operator<= (const Millisecond& other) const noexcept { return (operator==(other) == true) ? true: operator<(other); }

   bool operator<= (const Second& other) const noexcept { return (operator==(other) == true) ? true: operator<(other); }

   Microsecond& operator+= (const Microsecond& other) noexcept { m_value += other.m_value; return *this; }

   type_t getValue () const noexcept { return m_value; }

   static Microsecond getTime () noexcept;
   std::string asString () const noexcept;

   static Microsecond fromString (const std::string& value) throw (RuntimeException);

private:
   type_t m_value;

   friend class Millisecond;
   friend class Second;

   friend class Microsecond operator - (const Microsecond& left, const Microsecond& right) noexcept;
   friend class Microsecond operator + (const Microsecond& left, const Microsecond& right) noexcept;
   friend class Microsecond operator / (const Microsecond& left, const Microsecond& right) noexcept;
   friend class Microsecond operator / (const Microsecond& left, const int right) noexcept;
   friend class Microsecond operator / (const Microsecond& left, const unsigned int right) noexcept;
};

inline Microsecond operator - (const Microsecond& left, const Microsecond& right)
   noexcept
{
   return Microsecond (left.m_value - right.m_value);
}

inline Microsecond operator + (const Microsecond& left, const Microsecond& right)
   noexcept
{
   return Microsecond (left.m_value + right.m_value);
}

inline Microsecond operator / (const Microsecond& left, const Microsecond& right)
   noexcept
{
   return Microsecond (left.m_value / right.m_value);
}

inline Microsecond operator / (const Microsecond& left, const int right)
   noexcept
{
   return Microsecond (left.m_value / right);
}

inline Microsecond operator / (const Microsecond& left, const unsigned int right)
   noexcept
{
   return Microsecond (left.m_value / right);
}

}
}

#endif 
