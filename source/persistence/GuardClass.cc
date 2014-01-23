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
#include <wepa/persistence/GuardClass.hpp>
#include <wepa/persistence/Class.hpp>

#include <wepa/logger/Logger.hpp>

using namespace wepa;

persistence::GuardClass::GuardClass (Class& _class, const char* text) :
   m_class (_class),
   m_text (text)
{
   if (text == NULL) {
      LOG_DEBUG ("+++ Locking " << _class);
   }
   else {
      LOG_DEBUG ("+++ Locking " << _class << " for '" << m_text << "'");
   }

   m_class.m_mutex.lock ();

   if (m_class.m_members.empty() == true) {
      try {
         m_class.createMembers();
         if (m_class.m_members.empty() == true) {
            WEPA_THROW_EXCEPTION(m_class << " method createMembers does not define any member");
         }
      }
      catch (adt::RuntimeException& ex) {
         logger::Logger::write(ex);
      }
   }
}

persistence::GuardClass::~GuardClass ()
{
   if (m_text == NULL) {
      LOG_DEBUG ("+++ Unlocking " << m_class);
   }
   else {
      LOG_DEBUG ("+++ Unlocking " << m_class << " for '" << m_text << "'");
   }

   m_class.m_mutex.unlock ();
}

dbms::datatype::Abstract& persistence::GuardClass::getMember (const int columnNumber)
   throw (adt::RuntimeException)
{
   return std::ref (m_class.getMember (columnNumber));
}

const dbms::datatype::Abstract& persistence::GuardClass::getMember (const int columnNumber) const
   throw (adt::RuntimeException)
{
   return std::ref (m_class.getMember (columnNumber));
}

persistence::Object* persistence::GuardClass::createObject () noexcept
{
   return m_class.createObject();
}
