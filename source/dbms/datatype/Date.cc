#include <string.h>

#include <time.h>

#include <coffee/config/defines.hpp>

#include <coffee/adt/Second.hpp>

#include <coffee/dbms/datatype/Date.hpp>

using namespace std;
using namespace coffee;
using namespace coffee::dbms;

datatype::Date::Date (const char* name, const Constraint::_v constraint) :
   datatype::Abstract (name, Datatype::Date, MaxDateSize, constraint)
{
   datatype::Abstract::setBuffer (m_buffer);
   m_buffer [0] = 0;
   m_value = 0;
}

datatype::Date::Date (const char* name, const datatype::Abstract::Datatype::_v type,  const Constraint::_v constraint) :
   datatype::Abstract (name, type, MaxDateSize, constraint)
{
   datatype::Abstract::setBuffer (m_buffer);
   m_buffer [0] = 0;
   m_value = 0;
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

   adt::Second::type_t tt = m_value.getValue ();
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
   char* r = strptime (str, format, &aux);

   if (r == NULL ) {
      COFFEE_THROW_EXCEPTION("'" << str << "' is not a valid date using format '" << format << "'");
   }

   aux.tm_isdst = 0;
   time_t newValue = mktime (&aux);

   if (newValue == -1) {
      COFFEE_THROW_EXCEPTION("'" << str << "' can not be treated as adt::Second '" << format << "'");
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
   noexcept
{
   const char* cstring;

   adt::StreamString result ("datatype.Date { ");
   result << dbms::datatype::Abstract::asString ();
   result += " | Value: ";

   if (this->hasValue () == true)
      result << "'" << m_value.asString () << "'";
   else
      result += "<null>";

   return result += " }";
}

int datatype::Date::do_compare (const datatype::Abstract& other) const
   throw (adt::RuntimeException)
{
   const Date& _other = coffee_datatype_downcast(Date, other);

   return this->m_value - _other.m_value;
}

