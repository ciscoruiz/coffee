#include <coffee/adt/StreamString.hpp>
#include <coffee/adt/AsString.hpp>

#include <coffee/dbms/datatype/Float.hpp>

using namespace coffee;
using namespace coffee::dbms;

datatype::Float::Float (const char* name, const Constraint::_v constraint, const char* format) :
   datatype::Abstract (name, Datatype::Float, sizeof (float), constraint),
   m_format (format),
   m_value (0.0)
{
   datatype::Abstract::setBuffer (&m_value);
}

datatype::Float::Float (const Float& other) :
   datatype::Abstract (other),
   m_value (other.m_value),
   m_format (other.m_format)
{
   datatype::Abstract::setBuffer (&m_value);
}


adt::StreamString datatype::Float::asString () const
   noexcept
{
   adt::StreamString result;
   result <<  "datatype.Float { " << datatype::Abstract::asString ();
   result <<  " | Value: " << adt::AsString::apply (m_value, m_format);
   return result += " }";
}

int datatype::Float::do_compare (const datatype::Abstract& other) const
   throw (adt::RuntimeException)
{
   const Float& _other = coffee_datatype_downcast(Float, other);

   double rr = this->m_value - _other.m_value;

   if (rr == 0)
      return 0;
   else if (rr < 0.0)
      return -1;
   else
      return 1;
}
