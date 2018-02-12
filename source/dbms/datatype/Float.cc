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
