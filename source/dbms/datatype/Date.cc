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

datatype::Date::Date (const char* name, const bool isNulleable, const char* format) :
   datatype::Abstract (name, Datatype::Date, MaxDateSize, isNulleable)
{
   datatype::Abstract::setBuffer (m_buffer);
   m_buffer [0] = 0;
   m_format = (format == NULL) ? NULL: strdup (format);
   wepa_memset (&m_value, 0, sizeof (m_value));
}

datatype::Date::Date (const string& name, const bool isNulleable, const char* format) :
   datatype::Abstract (name, Datatype::Date, MaxDateSize, isNulleable)
{
   datatype::Abstract::setBuffer (m_buffer);
   m_buffer [0] = 0;
   m_format = (format == NULL) ? NULL: strdup (format);
   wepa_memset (&m_value, 0, sizeof (m_value));
}

datatype::Date::Date (const char* name, const datatype::Abstract::Datatype::_v type,  const bool isNulleable, const char* format) :
   datatype::Abstract (name, type, MaxDateSize, isNulleable)
{
   datatype::Abstract::setBuffer (m_buffer);
   m_buffer [0] = 0;
   m_format = (format == NULL) ? NULL: strdup (format);
   wepa_memset (&m_value, 0, sizeof (m_value));
}

datatype::Date::Date (const string& name, const datatype::Abstract::Datatype::_v type,  const bool isNulleable, const char* format) :
   datatype::Abstract (name, type, MaxDateSize, isNulleable)
{
   datatype::Abstract::setBuffer (m_buffer);
   m_buffer [0] = 0;
   m_format = (format == NULL) ? NULL: strdup (format);
   wepa_memset (&m_value, 0, sizeof (m_value));
}

datatype::Date::~Date ()
{
   if (m_format != NULL)
      free (m_format);
}

const char* datatype::Date::getCStringValue () const
   throw (adt::RuntimeException)
{
   this->exceptionWhenIsNull();

   const char* format;

   if ((format = m_format) == NULL)
      format = "%d/%m/%Y %H:%M:%S";

   return (strftime (const_cast <Date*> (this)->m_buffer, MaxDateSize, format, &m_value) == 0) ? NULL: m_buffer;
}

adt::Second datatype::Date::getSecondValue () const
throw (adt::RuntimeException)
{
   this->exceptionWhenIsNull();

   Date& _this = const_cast <Date&> (*this);

   int rr = mktime (&_this.m_value);

   if (rr == -1) {
      WEPA_THROW_EXCEPTION(asctime (&_this.m_value) << " can not be converted as adt::Second");
   }

   return adt::Second (rr);
}

void datatype::Date::setValue (const char* str)
   throw (adt::RuntimeException)
{
   if (m_format == NULL) {
      WEPA_THROW_EXCEPTION("This method requires a format specification");
   }

   tm aux;
   char* r = strptime (str, m_format, &aux);

   if (r == NULL) {
      WEPA_THROW_EXCEPTION("It is not a valid date");
   }

   datatype::Abstract::setNull (false);
   wepa_memcpy (&m_value, &aux, sizeof (m_value));
}

void datatype::Date::setValue (const adt::Second& second)
   throw (adt::RuntimeException)
{
   adt::Second::type_t tt = second.getValue();
   tm* aux = localtime ((time_t*) &tt);

   if (aux == NULL) {
      WEPA_THROW_EXCEPTION(asString () << second.asString() << " | It is not a valid date");
   }

   datatype::Abstract::setNull (false);
   wepa_memcpy (&m_value, aux, sizeof (m_value));
}

void datatype::Date::set (const char* what, int& variable, const int value, const int min, const int max)
   throw (adt::RuntimeException)
{
   if (value < min) {
      WEPA_THROW_EXCEPTION(what << " must be greater or equal to  " <<  min);
   }

   if (value > max && max != -1) {
      WEPA_THROW_EXCEPTION(what << " must be less or equal to " << max);
   }

   datatype::Abstract::setNull (false);
   variable = value;
}

adt::StreamString datatype::Date::asString () const
   throw ()
{
   const char* cstring;

   adt::StreamString result ("dbms::datatype::Date { ");
   result << dbms::datatype::Abstract::asString ();
   result << " | Format: ";
   result += (m_format == NULL) ? "<null>": m_format;
   result += " | Value: ";

   if (datatype::Abstract::isNull () == false) {
      if ((cstring = getCStringValue ()) == NULL)
         result += "<not valid>";
      else
         result += cstring;
   }
   else
      result += "<null>";

   return result += " }";
}

