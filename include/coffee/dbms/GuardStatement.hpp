// MIT License
// 
// Copyright (c) 2018 Francisco Ruiz (francisco.ruiz.rayo@gmail.com)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
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
