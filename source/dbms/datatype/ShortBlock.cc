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

#include <coffee/basis/AsString.hpp>

#include <coffee/dbms/datatype/ShortBlock.hpp>
#include <coffee/basis/AsHexString.hpp>

using namespace coffee;
using namespace coffee::dbms;

datatype::ShortBlock::ShortBlock (const char* name, const int maxSize, const Constraint::_v constraint) :
   datatype::Abstract (name, Datatype::ShortBlock, maxSize, constraint),
   m_value ()
{
   m_value.reserve(maxSize);
   datatype::Abstract::setBuffer ((void*) m_value.data ());
}

datatype::ShortBlock::ShortBlock (const ShortBlock& other) :
   datatype::Abstract (other),
   m_value ()
{
   m_value = other.m_value;
   datatype::Abstract::setBuffer ((void*) m_value.data ());
}

void datatype::ShortBlock::setValue(const basis::DataBlock& value)
   throw (basis::RuntimeException)
{
   if (value.size () > datatype::Abstract::getMaxSize ()) {
      COFFEE_THROW_EXCEPTION("Block out of range (" << datatype::Abstract::getMaxSize () << " and " << value.size ());
   }
//   coffee_memcpy(m_value.data (), value.data (), value.size ());
   m_value.assign(value.data (), value.size ());
   this->isNotNull();
}

basis::StreamString datatype::ShortBlock::asString () const
   noexcept
{
   basis::StreamString result ("datatype.ShortBlock { ");
   result += datatype::Abstract::asString ();
   result << " | MaxSize=" << getMaxSize();

   if (hasValue()) {
      result << " | Buffer=" << basis::AsHexString::apply((int64_t) getBuffer());
      result << " | Size=" << m_value.size ();
   }

   return result += " }";
}

int datatype::ShortBlock::do_compare (const datatype::Abstract& other) const
   throw (basis::RuntimeException)
{
   COFFEE_THROW_EXCEPTION(asString () << " | Can not apply");
   return 0;
}
