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

#ifndef __coffee_dbms_GuardConnection_hpp
#define __coffee_dbms_GuardConnection_hpp

#include <memory>
#include <functional>

#include <coffee/basis/RuntimeException.hpp>
#include <coffee/dbms/ResultCode.hpp>
#include <coffee/dbms/DatabaseException.hpp>

namespace coffee {
namespace dbms {

class Connection;
class GuardStatement;
class Statement;

class GuardConnection {
public:
   explicit GuardConnection (std::shared_ptr<Connection> connection) throw (basis::RuntimeException);
   ~GuardConnection ();

   std::shared_ptr<Connection>& operator-> () noexcept { return std::ref(m_connection); }

   int setMaxCommitPending (const int maxCommitPending) noexcept;
   void clearMaxCommitPending () noexcept;
   void commit() throw (basis::RuntimeException, DatabaseException);

   int getCountLinkedStatement () const noexcept { return m_countLinkedStatement; }

private:
   std::shared_ptr<Connection> m_connection;
   int m_countLinkedStatement;

   void linkStatement () noexcept { m_countLinkedStatement ++; }
   void unlinkStatement () noexcept { m_countLinkedStatement --; }

   ResultCode execute (std::shared_ptr<Statement>& statement) throw (basis::RuntimeException, DatabaseException);

   friend class GuardStatement;
};
}
}

#endif
