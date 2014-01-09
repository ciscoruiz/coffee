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
#include <string.h>

#include <time.h>

#include <wepa/config/defines.hpp>

#include <wepa/adt/Second.hpp>

#include <wepa/dbms/datatype/Date.hpp>

using namespace std;
using namespace wepa;
using namespace wepa::dbms;

datatype::Date::Date (const char* name, const bool isNulleable) :
   datatype::Abstract (name, Datatype::Date, MaxDateSize, isNulleable)
{
   datatype::Abstract::setBuffer (m_buffer);
   m_buffer [0] = 0;
   wepa_memset (&m_value, 0, sizeof (m_value));
}

datatype::Date::Date (const string& name, const bool isNulleable) :
   datatype::Abstract (name, Datatype::Date, MaxDateSize, isNulleable)
{
   datatype::Abstract::setBuffer (m_buffer);
   m_buffer [0] = 0;
   wepa_memset (&m_value, 0, sizeof (m_value));
}

datatype::Date::Date (const char* name, const datatype::Abstract::Datatype::_v type,  const bool isNulleable) :
   datatype::Abstract (name, type, MaxDateSize, isNulleable)
{
   datatype::Abstract::setBuffer (m_buffer);
   m_buffer [0] = 0;
   wepa_memset (&m_value, 0, sizeof (m_value));
}

datatype::Date::Date (const string& name, const datatype::Abstract::Datatype::_v type,  const bool isNulleable) :
   datatype::Abstract (name, type, MaxDateSize, isNulleable)
{
   datatype::Abstract::setBuffer (m_buffer);
   m_buffer [0] = 0;
   wepa_memset (&m_value, 0, sizeof (m_value));
}

datatype::Date::~Date ()
{
}

struct tm* datatype::Date::getLocalTime () const
   throw (adt::RuntimeException)
{
   this->exceptionWhenIsNull();

   adt::Second::type_t tt = m_value.getValue ();
   tm* result = localtime ((time_t*) &tt);

   if (result == NULL) {
      WEPA_THROW_EXCEPTION(asString () << " | It is not a valid date");
   }

   return result;
}

void datatype::Date::setValue (const char* str, const char* format)
   throw (adt::RuntimeException)
{
   clear ();

   if (format == NULL) {
      WEPA_THROW_EXCEPTION("This method requires a format specification");
   }

   tm aux;
   char* r = strptime (str, format, &aux);

   if (r == NULL) {
      WEPA_THROW_EXCEPTION("'" << str << "' is not a valid date using format '" << format << "'");
   }

   time_t newValue = mktime (&aux);

   if (newValue == -1) {
      WEPA_THROW_EXCEPTION("'" << str << "' can not be treated as adt::Second '" << format << "'");
   }

   this->isNotNull();
   m_value = newValue;
}

void datatype::Date::setValue (const adt::Second& value)
   throw (adt::RuntimeException)
{
   this->isNotNull();
   m_value = value;
}

adt::StreamString datatype::Date::asString () const
   throw ()
{
   const char* cstring;

   adt::StreamString result ("dbms::datatype::Date { ");
   result << dbms::datatype::Abstract::asString ();
   result += "' | Value: ";

   if (this->hasValue () == true)
      result << m_value.asString ();
   else
      result += "<null>";

   return result += " }";
}

