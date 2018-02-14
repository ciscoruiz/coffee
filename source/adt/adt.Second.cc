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
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

#include <coffee/adt/AsString.hpp>
#include <coffee/adt/Second.hpp>
#include <coffee/adt/Millisecond.hpp>
#include <coffee/adt/Microsecond.hpp>

using namespace std;
using namespace coffee;

#define implement_operator(op) \
   bool adt::Second::operator op (const Millisecond& other) const \
      noexcept\
   {\
      return m_value op (other.m_value / 1000);\
   }\
   bool adt::Second::operator op (const Microsecond& other) const\
      noexcept\
   {\
      return m_value op (other.m_value / 1000000);\
   }

adt::Second::Second (const adt::Millisecond& other) : m_value (other.m_value / 1000) {;}

adt::Second::Second (const adt::Microsecond& other) : m_value (other.m_value / 1000000) {;}

adt::Second& adt::Second::operator= (const adt::Millisecond& other)
   noexcept
{
   m_value = (other.m_value / 1000);
   return *this;
}

adt::Second& adt::Second::operator= (const adt::Microsecond& other)
   noexcept
{
   m_value = (other.m_value / 1000000);
   return *this;
}

implement_operator (==)
implement_operator (!=)
implement_operator (>)
implement_operator (<)

string adt::Second::asDateTime (const char* format) const
   throw (RuntimeException)
{
   char aux [DateTimeSizeString];

   time_t time = m_value;

   struct tm* tt = localtime (&time);

   if (tt == nullptr) {
	   COFFEE_THROW_EXCEPTION(m_value << " is a bad time");
   }

   if (strftime (aux, sizeof (aux), format, tt) == 0) {
	   COFFEE_THROW_EXCEPTION(m_value << "is a bad date");
   }

   return string(aux);
}

//static
adt::Second adt::Second::getTime ()
   noexcept
{
   struct timeval tv;
   gettimeofday (&tv, NULL);
   return adt::Second (tv.tv_sec);
}

//static
adt::Second adt::Second::getLocalTime ()
   noexcept
{
   return adt::Second (time (NULL));
}

string adt::Second::asString () const
   noexcept
{
   string result (AsString::apply (m_value));
   return result += " sec";
}

//static
adt::Second adt::Second::fromString (const std::string& value)
   throw (adt::RuntimeException)
{
   if (value.find (" sec") == string::npos)
      COFFEE_THROW_EXCEPTION ("String: " << value << " is not a valid second expression");

   return adt::Second (atol (value.c_str()));
}

//static
adt::Second adt::Second::fromString(const std::string& text, const char* format)
   throw(RuntimeException)
{
   tm gmtime;
   const char* aux;

   if (strptime(text.c_str(), format, &gmtime) == NULL) {
      COFFEE_THROW_EXCEPTION(text << " is not valid expression for " << format);
   }

   time_t now = time(NULL);
   tm* localtm = localtime(&now);
   return Second(mktime(&gmtime) + localtm->tm_gmtoff);
}

