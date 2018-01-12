// WEPA - Write Excellent Professional Applications
//
//(c) Copyright 2013 Francisco Ruiz Rayo
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#include <memory>

#include <wepa/dbms/datatype/Abstract.hpp>

#include <wepa/persistence/PrimaryKey.hpp>
#include <wepa/persistence/PrimaryKeyBuilder.hpp>

using namespace wepa;

persistence::PrimaryKey::PrimaryKey(const PrimaryKeyBuilder& builder)
{
    m_components = builder.getComponents();
}

int persistence::PrimaryKey::compareTo(const PrimaryKey& other) const
   throw(adt::RuntimeException)
{
   if(this == &other)
      return 0;

   if(m_components.size() != other.m_components.size()) {
      WEPA_THROW_EXCEPTION(
         "Can not compare primary keys with different number of components | N-Size=" << m_components.size() << " | other-Size=" << other.m_components.size()
      );
   }

   int result = 0;

   for(int ii = 0, maxii = m_components.size(); ii < maxii && result == 0; ++ ii) {
      auto thisComponent = m_components[ii];
      auto otherComponent = other.m_components[ii];
      result = thisComponent->compare(std::ref(*otherComponent));
   }

   return result;
}

const persistence::PrimaryKey::Component& persistence::PrimaryKey::getComponent(const int pos) const
   throw(adt::RuntimeException)
{
   if(pos >= m_components.size()) {
      WEPA_THROW_EXCEPTION("Position " << pos << " is out of range");
   }

   return m_components[pos];
}

persistence::PrimaryKey& persistence::PrimaryKey::operator=(const PrimaryKey& other)
   throw(adt::Exception)
{
   if(this == &other)
      return std::ref(*this);

   m_components.clear();
   
   for(const auto& ii : other.m_components) {
      m_components.push_back(ii->clone());
   }

   return *this;
}

adt::StreamString persistence::PrimaryKey::asString() const noexcept
{
   adt::StreamString result("persistence.PrimaryKey { ");

   for(const auto& component: m_components) {
      result += component->asString();
      result += "  ";
   }

   return result += "}";
}

