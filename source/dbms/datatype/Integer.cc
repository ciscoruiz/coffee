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
