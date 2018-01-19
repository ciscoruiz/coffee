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
#ifndef __wepa_persistence_Recorder_hpp
#define __wepa_persistence_Recorder_hpp

#include <wepa/persistence/Accessor.hpp>
#include <wepa/persistence/Object.hpp>

#include <wepa/adt/RuntimeException.hpp>

namespace wepa {

namespace dbms {
   class GuardStatement;
}
namespace persistence {

class Object;

class Recorder : public Accessor {
public:
   virtual ~Recorder () {;}

   const Accessor::TheObject& getObject() const noexcept { return m_object; }

   virtual dbms::ResultCode apply(dbms::GuardStatement& statement)
      throw(adt::RuntimeException, dbms::DatabaseException) = 0;

protected:
   Recorder (const char* name, TheStatement& statement, const Accessor::TheObject& object) :
      Accessor(name, statement, object->getPrimaryKey()),
      m_object(object)
   {;}

private:
   Accessor::TheObject m_object;
};

} /* namespace persistence */
} /* namespace wepa */
#endif
