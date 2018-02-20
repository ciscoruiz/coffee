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

