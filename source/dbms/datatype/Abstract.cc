#include <functional>

#include <coffee/adt/StreamString.hpp>

#include <coffee/dbms/datatype/Abstract.hpp>

using namespace coffee;
using namespace coffee::dbms;

adt::StreamString datatype::Abstract::asString () const
   noexcept
{
   adt::StreamString result;

   result << "datatype.Abstract { Name: " << m_name;
   result << " | Null: " << m_isNull;
   result << " | Constraint: " << ((m_constraint == Constraint::CanBeNull) ? "CanBeNull": "CanNotBeNull");

   return result += " }";
}

void datatype::Abstract::isNull ()
   throw (adt::RuntimeException)
{
   if (m_constraint == Constraint::CanNotBeNull) {
      COFFEE_THROW_EXCEPTION(asString () << " | Data can not be NULL");
   }

   m_isNull = true;
}

void datatype::Abstract::clear ()
   noexcept
{
   if (m_constraint == Constraint::CanBeNull)
      m_isNull = true;

   do_clear ();
}

void datatype::Abstract::exceptionWhenIsNull () const
   throw (adt::RuntimeException)
{
   if (m_isNull == true) {
      COFFEE_THROW_EXCEPTION("Data '" << m_name << "' is null and it can not return any value");
   }
}

int datatype::Abstract::compare (const std::shared_ptr<Abstract>& other) const
   throw (adt::RuntimeException)
{
   return compare(std::ref(*other.get()));
}

int datatype::Abstract::compare (const Abstract& other) const
   throw (adt::RuntimeException)
{
   if (this->getType () != other.getType()) {
      COFFEE_THROW_EXCEPTION(this->asString () << " type does not matches with " << other.asString());
   }

   if (this->hasValue() == false && other.hasValue () == false)
      return 0;

   if (this->hasValue () == true && other.hasValue () == false)
      return 1;

   if (this->hasValue () == false && other.hasValue () == true)
      return -1;

   return this->do_compare (other);
}
