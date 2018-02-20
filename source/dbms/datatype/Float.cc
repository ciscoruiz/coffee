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
