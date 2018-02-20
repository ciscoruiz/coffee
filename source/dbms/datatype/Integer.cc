#include <coffee/dbms/datatype/Integer.hpp>

using namespace coffee;
using namespace coffee::dbms;

datatype::Integer::Integer (const char* name, const Constraint::_v constraint) :
   datatype::Abstract (name, Datatype::Integer, sizeof (int), constraint),
   m_value (0)
{
   datatype::Abstract::setBuffer (&m_value);
}

datatype::Integer::Integer (const Integer& other) :
   datatype::Abstract (other),
   m_value (other.m_value)
{
   datatype::Abstract::setBuffer (&m_value);
}

adt::StreamString datatype::Integer::asString () const
   noexcept
{
   adt::StreamString result;
   result << "datatype.Integer { " << datatype::Abstract::asString ();
   result <<  " | Value: " << m_value;
   return result += " }";
}

int datatype::Integer::do_compare (const datatype::Abstract& other) const
   throw (adt::RuntimeException)
{
   const Integer& _other = coffee_datatype_downcast(Integer, other);

   return this->m_value - _other.m_value;
}
