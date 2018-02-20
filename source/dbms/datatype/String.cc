#include <coffee/config/defines.hpp>

#include <coffee/dbms/datatype/String.hpp>

using namespace coffee;
using namespace coffee::dbms;

datatype::String::String (const char* name, const int maxSize, const Constraint::_v constraint) :
   datatype::Abstract (name, Datatype::String, maxSize, constraint)
{
   datatype::Abstract::setBuffer (m_value = new char [maxSize + 1]);
   coffee_memset (m_value, 0, maxSize + 1);
}

datatype::String::String (const String& other) :
   datatype::Abstract (other)
{
   const int maxSize = getMaxSize ();
   datatype::Abstract::setBuffer (m_value = new char [maxSize + 1]);
   coffee_memcpy (m_value, other.m_value, maxSize);
}

void datatype::String::setValue (const char* str)
   throw (adt::RuntimeException)
{
   if (str == NULL) {
      this->isNull ();
      clear ();
      return;
   }

   if (m_value != str) {
      auto len = coffee_strlen (str);
      if (len > datatype::Abstract::getMaxSize ()) {
         COFFEE_THROW_EXCEPTION("'" << str << "' out of range | MaxLen=" << datatype::Abstract::getMaxSize () << " | Len=" << len);
      }
      coffee_strcpy (m_value, str);
   }

   this->isNotNull();
}

adt::StreamString datatype::String::asString () const
   noexcept
{
   adt::StreamString result ("datatype.String { ");
   result += datatype::Abstract::asString ();
   result << " | MaxSize: " << getMaxSize();
   result += " | Value: ";

   if (this->hasValue () == true)
      result << "'" << m_value << "'";
   else
      result += "<null>";

   return result += " }";
}

int datatype::String::do_compare (const datatype::Abstract& other) const
   throw (adt::RuntimeException)
{
   const String& _other = coffee_datatype_downcast(String, other);

   return coffee_strcmp (this->m_value, _other.m_value);
}

