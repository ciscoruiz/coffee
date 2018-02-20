#include <coffee/dbms/datatype/LongBlock.hpp>
#include <coffee/adt/AsHexString.hpp>

using namespace coffee;
using namespace coffee::dbms;

datatype::LongBlock::LongBlock (const char* name, const Constraint::_v constraint) :
   datatype::Abstract (name, Datatype::LongBlock, 0, constraint),
   m_value ()
{
   datatype::Abstract::setBuffer ((void*) NULL);
}

datatype::LongBlock::LongBlock (const LongBlock& other) :
   datatype::Abstract (other),
   m_value ()
{
   m_value = other.m_value;
}

void datatype::LongBlock::setValue (const adt::DataBlock& value)
   throw (adt::RuntimeException)
{
   m_value = value;
   this->isNotNull();
}

adt::StreamString datatype::LongBlock::asString () const
   noexcept
{
   adt::StreamString result ("datatype.LongBlock { ");
   result += datatype::Abstract::asString ();
   result << " | Buffer: " << adt::AsHexString::apply(coffee_ptrnumber_cast (getBuffer()));
   result << " | MaxSize: " << getMaxSize();
   result += " | Size: ";
   if (this->hasValue () == true)
      result << m_value.size ();
   else
      result += "(null)";
   return result += " }";
}

int datatype::LongBlock::do_compare (const datatype::Abstract& other) const
   throw (adt::RuntimeException)
{
   COFFEE_THROW_EXCEPTION(asString () << " | Can not apply");
   return 0;
}
