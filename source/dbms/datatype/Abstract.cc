// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2013 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/wepa
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
#include <functional>

#include <wepa/adt/StreamString.hpp>

#include <wepa/adt/AsHexString.hpp>

#include <wepa/dbms/datatype/Abstract.hpp>

using namespace wepa;
using namespace wepa::dbms;

adt::StreamString datatype::Abstract::asString () const
   noexcept
{
   adt::StreamString result;

   result << "datatype.Abstract { Name: " << m_name;
   result << " | Buffer: " << adt::AsHexString::apply(wepa_ptrnumber_cast (m_buffer));
   result << " | MaxSize: " << m_maxSize;
   result << " | Null: " << m_isNull;
   result << " | Constraint: " << ((m_constraint == Constraint::CanBeNull) ? "CanBeNull": "CanNotBeNull");

   return result += " }";
}

void datatype::Abstract::isNull ()
   throw (adt::RuntimeException)
{
   if (m_constraint == Constraint::CanNotBeNull) {
      WEPA_THROW_EXCEPTION(asString () << " | Data can not be NULL");
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
   throw (adt::RuntimeException)
{
   if (m_isNull == true) {
      WEPA_THROW_EXCEPTION("Data '" << m_name << "' is null and it can not return any value");
   }
}

int datatype::Abstract::compare (const std::shared_ptr<Abstract>& other) const
   throw (adt::RuntimeException)
{
   return compare(std::ref(*other.get()));
}

int datatype::Abstract::compare (const Abstract& other) const
   throw (adt::RuntimeException)
{
   if (this->getType () != other.getType()) {
      WEPA_THROW_EXCEPTION(this->asString () << " type does not matches with " << other.asString());
   }

   if (this->hasValue() == false && other.hasValue () == false)
      return 0;

   if (this->hasValue () == true && other.hasValue () == false)
      return 1;

   if (this->hasValue () == false && other.hasValue () == true)
      return -1;

   return this->do_compare (other);
}
