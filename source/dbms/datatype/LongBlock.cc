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

#include <coffee/dbms/datatype/LongBlock.hpp>
#include <coffee/basis/AsHexString.hpp>

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

void datatype::LongBlock::setValue (const basis::DataBlock& value)
   throw (basis::RuntimeException)
{
   m_value = value;
   this->isNotNull();
}

basis::StreamString datatype::LongBlock::asString () const
   noexcept
{
   basis::StreamString result ("datatype.LongBlock { ");
   result += datatype::Abstract::asString ();
   result << " | MaxSize=" << getMaxSize();

   if (hasValue()) {
      result << " | Buffer=" << basis::AsHexString::apply((int64_t) getBuffer());
      result << " | Size=" << m_value.size ();
   }

   return result += " }";
}

int datatype::LongBlock::do_compare (const datatype::Abstract& other) const
   throw (basis::RuntimeException)
{
   COFFEE_THROW_EXCEPTION(asString () << " | Can not apply");
   return 0;
}
