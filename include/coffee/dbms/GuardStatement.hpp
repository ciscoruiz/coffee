// COFFEE - COmpany eFFEEctive Platform
//
//(c) Copyright 2018 Francisco Ruiz Rayo
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#ifndef __coffee_dbms_GuardStatement_hpp
#define __coffee_dbms_GuardStatement_hpp

#include <memory>

#include <coffee/adt/RuntimeException.hpp>
#include <coffee/dbms/ResultCode.hpp>

namespace coffee {
namespace dbms {

namespace datatype {
class Abstract;
}

class GuardConnection;

class Statement;

/**
 * This class will be previously protected by the GuardConnection it will assure that it will no be possible
 * a deadlock.
 */
class GuardStatement {
public:
   GuardStatement(GuardConnection& guardConnection, std::shared_ptr<Statement>&) throw(adt::RuntimeException);
   ~GuardStatement();

   GuardConnection& getGuardConnection() noexcept { return m_guardConnection; }
   std::shared_ptr<Statement> operator->() noexcept { return m_statement; }

   std::shared_ptr<datatype::Abstract>& getInputData(const int pos) throw(adt::RuntimeException);
   const std::shared_ptr<datatype::Abstract>& getOutputData(const int pos) const throw(adt::RuntimeException);

   ResultCode execute() throw(adt::RuntimeException, DatabaseException);

   bool fetch() throw(adt::RuntimeException, DatabaseException);

   bool setRequiresCommit(const bool requiresCommit) noexcept;

private:
   GuardConnection& m_guardConnection;
   std::shared_ptr<Statement> m_statement;
};
}
}

#endif
