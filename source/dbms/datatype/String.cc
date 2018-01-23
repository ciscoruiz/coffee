// COFFEE - COmpany eFFEEctive Platform
//
// (c) Copyright 2018 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/coffee
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
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

datatype::String::String (const std::string& name, const int maxSize, const Constraint::_v constraint) :
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

char* datatype::String::strip (char *str)
   noexcept
{
   int len;

   if (str == NULL || (len = coffee_strlen (str)) == 0)
      return str;

   register int end = len - 1;

   while (end >= 0 && str [end] == ' ') end --;

   if (end >= 0)
      str [++ end] = 0;

   return str;
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

