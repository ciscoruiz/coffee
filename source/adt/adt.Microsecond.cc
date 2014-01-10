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
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

#include <wepa/adt/AsString.hpp>
#include <wepa/adt/Millisecond.hpp>
#include <wepa/adt/Second.hpp>
#include <wepa/adt/Microsecond.hpp>

using namespace std;
using namespace wepa;

#define implement_operator(op) \
   bool adt::Microsecond::operator op (const adt::Millisecond& other) const \
      noexcept\
   {\
      return (m_value / 1000) op (((type_t) other.m_value));\
   }\
   bool adt::Microsecond::operator op (const adt::Second& other) const\
      noexcept\
   {\
      return (m_value / 1000000) op ((type_t) other.m_value);\
   }

adt::Microsecond::Microsecond (const adt::Millisecond& other) : m_value (other.m_value) { m_value *= 1000; }

adt::Microsecond::Microsecond (const adt::Second& other) : m_value (other.m_value) { m_value *= 1000000; }

adt::Microsecond& adt::Microsecond::operator= (const adt::Millisecond& other)
   noexcept
{
   m_value = other.m_value;
   m_value *= 1000;
   return *this;
}

adt::Microsecond& adt::Microsecond::operator= (const adt::Second& other)
   noexcept
{
   m_value = other.m_value;
   m_value *= 1000000;
   return *this;
}

implement_operator (==)
implement_operator (!=)
implement_operator (>)
implement_operator (<)


//static
adt::Microsecond adt::Microsecond::getTime ()
   noexcept
{
   struct timeval tv;
   gettimeofday (&tv, NULL);
   Microsecond result (Second (tv.tv_sec));
   result.m_value += tv.tv_usec;
   return result;
}

string adt::Microsecond::asString () const
   noexcept
{
   string result (AsString::apply (m_value));
   return result += " us";
}

//static
adt::Microsecond adt::Microsecond::fromString (const std::string& value)
   throw (adt::RuntimeException)
{
   if (value.find (" us") == string::npos)
      WEPA_THROW_EXCEPTION ("String: " << value << " is not a valid microsecond expression");

   return Microsecond (atoll (value.c_str ()));
}
