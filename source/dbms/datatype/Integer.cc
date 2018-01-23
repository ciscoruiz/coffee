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
#include <coffee/dbms/datatype/Integer.hpp>

using namespace coffee;
using namespace coffee::dbms;

datatype::Integer::Integer (const char* name, const Constraint::_v constraint) :
   datatype::Abstract (name, Datatype::Integer, sizeof (int), constraint),
   m_value (0)
{
   datatype::Abstract::setBuffer (&m_value);
}

datatype::Integer::Integer (const std::string& name, const Constraint::_v constraint) :
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
