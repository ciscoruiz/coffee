// WEPA - Write Excellent Professional Applications
//
//(c) Copyright 2018 Francisco Ruiz Rayo
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
#ifndef __wepa_persistence_Object_hpp
#define __wepa_persistence_Object_hpp

#include <functional>

#include <wepa/adt/StreamString.hpp>
#include <wepa/adt/RuntimeException.hpp>

#include <wepa/dbms/datatype/Set.hpp>

namespace wepa {
   
namespace persistence {

class PrimaryKey;
class Class;


//// See http://stackoverflow.com/questions/1319876/weird-c-templating-issues
//template <typename _T> class AutoObject;

class Object : public dbms::datatype::Set {
public:
   Object(const Class& _class, const std::shared_ptr<PrimaryKey>& primaryKey, const dbms::datatype::Set& members);

   ~Object() {
       m_primaryKey.reset();
   }

   const Class& getClass() const noexcept { return std::ref(m_class); }
   const std::shared_ptr<PrimaryKey>& getPrimaryKey() const throw(adt::RuntimeException) { return m_primaryKey; }

   std::string getInternalId() const noexcept;
   
   operator adt::StreamString() const noexcept { return asString(); }

   adt::StreamString asString() const noexcept;

private:
   const Class& m_class;
   std::shared_ptr<PrimaryKey> m_primaryKey;
};

} /* namespace persistence */
} /* namespace wepa */
#endif
