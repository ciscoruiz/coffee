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
#include <coffee/persistence/ClassBuilder.hpp>
#include <coffee/persistence/PrimaryKey.hpp>

using namespace coffee;

persistence::ClassBuilder& persistence::ClassBuilder::set(std::shared_ptr<PrimaryKey>& primaryKey)
throw (adt::RuntimeException)
{
   if (m_primaryKey) {
      COFFEE_THROW_EXCEPTION(m_className << " primary key already added");
   }

   m_primaryKey = primaryKey;
   return *this;
}

std::shared_ptr<persistence::Class> persistence::ClassBuilder::build() const
      throw (adt::RuntimeException)
{
   if (!m_primaryKey) {
      COFFEE_THROW_EXCEPTION(m_className << " does not define a valid primary key");
   }

   if (empty()) {
      COFFEE_THROW_EXCEPTION(m_className << " does not define any member");
   }

   for (const_data_iterator ii = begin(), maxii = end(); ii != maxii; ++ ii) {
      if (m_primaryKey->constains(name(ii)) == true) {
         COFFEE_THROW_EXCEPTION(m_className << " member " << name(ii) << " already used as a member in the primary key");
      }
   }

   return std::make_shared<persistence::Class>(*this);
}
