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
#include <stdio.h>

#include <wepa/adt/AsString.h>
#include <wepa/adt/Second.h>
#include <wepa/adt/Millisecond.h>
#include <wepa/adt/Microsecond.h>

using namespace std;
using namespace wepa;

#define implement_operator(op) \
   bool adt::Second::operator op (const Millisecond& other) const \
      throw ()\
   {\
      return m_value op (other.m_value / 1000);\
   }\
   bool adt::Second::operator op (const Microsecond& other) const\
      throw ()\
   {\
      return m_value op (other.m_value / 1000000);\
   }

adt::Second::Second (const adt::Millisecond& other) : m_value (other.m_value / 1000) {;}

adt::Second::Second (const adt::Microsecond& other) : m_value (other.m_value / 1000000) {;}

adt::Second& adt::Second::operator= (const adt::Millisecond& other)
   throw ()
{
   m_value = (other.m_value / 1000);
   return *this;
}

adt::Second& adt::Second::operator= (const adt::Microsecond& other)
   throw ()
{
   m_value = (other.m_value / 1000000);
   return *this;
}

implement_operator (==)
implement_operator (!=)
implement_operator (>)
implement_operator (<)

string adt::Second::asDateTime (const char* format) const
   throw ()
{
   char aux [DateTimeSizeString];

   return string (asDateTime (aux, format));
}

const char* adt::Second::asDateTime (char* result, const char* format) const
   throw ()
{
   struct tm* tt = localtime ((time_t*) &m_value);
   char aux [256];

   if (strftime (aux, sizeof (aux), format, tt) == 0)
      wepa_strcpy (aux, "Bad date");

   return wepa_strcpy (result, aux);
}

//static
adt::Second adt::Second::getTime ()
   throw ()
{
   struct timeval tv;
   gettimeofday (&tv, NULL);
   return adt::Second (tv.tv_sec);
}

//static
adt::Second adt::Second::getLocalTime ()
   throw ()
{
   return adt::Second (time (NULL));
}

string adt::Second::asString () const
   throw ()
{
   string result (AsString::apply (m_value));
   return result += " sec";
}

//static
adt::Second adt::Second::fromString (const std::string& value)
   throw (adt::RuntimeException)
{
   if (value.find (" sec") == string::npos)
      WEPA_THROW_EXCEPTION ("String: " << value << " is not a valid second expression");

   return adt::Second (atol (value.c_str()));
}

