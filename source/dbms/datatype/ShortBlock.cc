// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2018 Francisco Ruiz Rayo
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
#include <wepa/adt/AsString.hpp>

#include <wepa/dbms/datatype/ShortBlock.hpp>
#include <wepa/adt/AsHexString.hpp>

using namespace wepa;
using namespace wepa::dbms;

datatype::ShortBlock::ShortBlock (const char* name, const int maxSize, const Constraint::_v constraint) :
   datatype::Abstract (name, Datatype::ShortBlock, maxSize, constraint),
   m_value ()
{
   m_value.reserve(maxSize);
   datatype::Abstract::setBuffer ((void*) m_value.data ());
}

datatype::ShortBlock::ShortBlock (const std::string& name, const int maxSize, const Constraint::_v constraint) :
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

void datatype::ShortBlock::setValue(const adt::DataBlock& value)
   throw (adt::RuntimeException)
{
   if (value.size () > datatype::Abstract::getMaxSize ()) {
      WEPA_THROW_EXCEPTION("Block out of range (" << datatype::Abstract::getMaxSize () << " and " << value.size ());
   }
//   wepa_memcpy(m_value.data (), value.data (), value.size ());
   m_value.assign(value.data (), value.size ());
   this->isNotNull();
}

adt::StreamString datatype::ShortBlock::asString () const
   noexcept
{
   adt::StreamString result ("datatype.ShortBlock { ");
   result += datatype::Abstract::asString ();
   result << " | Buffer: " << adt::AsHexString::apply(wepa_ptrnumber_cast (getBuffer()));
   result << " | MaxSize: " << getMaxSize();
   result += " | Value: ";

   if (this->hasValue() == true)
      result << adt::AsString::apply (m_value);
   else
      result += "(null)";

   return result += " }";
}

int datatype::ShortBlock::do_compare (const datatype::Abstract& other) const
   throw (adt::RuntimeException)
{
   WEPA_THROW_EXCEPTION(asString () << " | Can not apply");
   return 0;
}
