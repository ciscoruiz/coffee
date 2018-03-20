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

#include <string.h>

#include <time.h>

#include <coffee/config/defines.hpp>

#include <coffee/dbms/datatype/Date.hpp>

using namespace std;
using namespace coffee;
using namespace coffee::dbms;

using std::chrono::seconds;

//static
const char* datatype::Date::DefaultFormat = "%Y-%m-%dT%H:%M:%S";

datatype::Date::Date (const char* name, const Constraint::_v constraint) :
   datatype::Abstract (name, Datatype::Date, MaxDateSize, constraint),
   m_value(seconds::zero())
{
   datatype::Abstract::setBuffer (m_buffer);
   m_buffer [0] = 0;
}

datatype::Date::Date (const char* name, const datatype::Abstract::Datatype::_v type,  const Constraint::_v constraint) :
   datatype::Abstract (name, type, MaxDateSize, constraint),
   m_value(seconds::zero())
{
   datatype::Abstract::setBuffer (m_buffer);
   m_buffer [0] = 0;
}

datatype::Date::Date (const Date& other) :
   datatype::Abstract (other),
   m_value (other.m_value)
{
   datatype::Abstract::setBuffer (m_buffer);
   m_buffer [0] = 0;
}

struct tm* datatype::Date::getLocalTime () const
   throw (adt::RuntimeException)
{
   this->exceptionWhenIsNull();

   time_t tt = m_value.count();
   tm* result = localtime ((time_t*) &tt);

   if (result == NULL) {
      COFFEE_THROW_EXCEPTION(asString () << " | It is not a valid date");
   }

   return result;
}

void datatype::Date::setValue (const char* str, const char* format)
   throw (adt::RuntimeException)
{
   clear ();

   if (format == NULL) {
      COFFEE_THROW_EXCEPTION("This method requires a format specification");
   }

   tm aux;
   coffee_memset(&aux, 0, sizeof(aux));
   char* r = strptime (str, format, &aux);

   if (r == NULL ) {
      COFFEE_THROW_EXCEPTION("'" << str << "' is not a valid date using format '" << format << "'");
   }

   aux.tm_isdst = 0;
   std::time_t newValue = std::mktime (&aux);

   if (newValue == -1) {
      COFFEE_THROW_EXCEPTION("'" << str << "' can not be treated as std::chrono::seconds '" << format << "'");
   }

   setValue(std::chrono::seconds(newValue));
}

void datatype::Date::setValue (const std::chrono::seconds& value)
   throw (adt::RuntimeException)
{
   this->isNotNull();
   m_value = value;
}

adt::StreamString datatype::Date::asString () const
   noexcept
{
   const char* cstring;

   adt::StreamString result ("datatype.Date { ");
   result << dbms::datatype::Abstract::asString ();

   if (this->hasValue () == true) {
      result << " | Value=" << "'" << m_value << "'";
   }

   return result += " }";
}

int datatype::Date::do_compare (const datatype::Abstract& other) const
   throw (adt::RuntimeException)
{
   const Date& _other = coffee_datatype_downcast(Date, other);

   return this->m_value.count() - _other.m_value.count();
}

