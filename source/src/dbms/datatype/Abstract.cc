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

#include <functional>

#include <coffee/basis/StreamString.hpp>

#include <coffee/dbms/datatype/Abstract.hpp>

using namespace coffee;
using namespace coffee::dbms;

basis::StreamString datatype::Abstract::asString () const
   noexcept
{
   basis::StreamString result;

   result << "datatype.Abstract { Name=" << m_name;
   result << " | IsNull=" << m_isNull;
   result << " | Constraint=" << ((m_constraint == Constraint::CanBeNull) ? "CanBeNull": "CanNotBeNull");

   return result += " }";
}

void datatype::Abstract::isNull ()
   throw (basis::RuntimeException)
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
   throw (basis::RuntimeException)
{
   if (m_isNull == true) {
      COFFEE_THROW_EXCEPTION("Data '" << m_name << "' is null and it can not return any value");
   }
}

int datatype::Abstract::compare (const std::shared_ptr<Abstract>& other) const
   throw (basis::RuntimeException)
{
   return compare(std::ref(*other.get()));
}

int datatype::Abstract::compare (const Abstract& other) const
   throw (basis::RuntimeException)
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
