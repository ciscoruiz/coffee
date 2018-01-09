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
#ifndef __wepa_persistence_ClassBuilder_hpp
#define __wepa_persistence_ClassBuilder_hpp

#include <memory>

#include <wepa/adt/RuntimeException.hpp>

#include <wepa/persistence/Class.hpp>

namespace wepa {

namespace persistence {

class ClassBuilder {
public:
   ClassBuilder(const std::string& className) : m_className (className) {;}
   
   ClassBuilder& addPrimaryKeyCompoment(Class::Data& data) noexcept {
      m_members.push_back(Class::Member(data, true));
      return *this;
   }
   
   ClassBuilder& addMember(Class::Data& data) noexcept {
      m_members.push_back(Class::Member(data, false));
      return *this;
   }

   const std::string& getClassName() const noexcept { return m_className; }
   const Class::Members& getMembers() const noexcept { return m_members; }

   std::shared_ptr<Class> build() const throw (adt::RuntimeException);
   
private:
   const std::string m_className;
   Class::Members m_members;
};

}
}
#endif
