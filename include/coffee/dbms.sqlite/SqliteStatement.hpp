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
#ifndef _coffee_dbms_sqlite_SqliteStatement_hpp_
#define _coffee_dbms_sqlite_SqliteStatement_hpp_

#include <sqlite3.h>
#include <coffee/dbms/Statement.hpp>

namespace coffee {
namespace dbms {
namespace sqlite {

class SqliteStatement : public Statement {
public:
   SqliteStatement(const Database& database, const char* name, const std::string& expression, const ActionOnError::_v actionOnError);
   virtual ~SqliteStatement();

   ::sqlite3_stmt* getImpl() { return impl; }

   Connection* getOwner() noexcept { return owner; }

private:
   ::sqlite3_stmt* impl;
   Connection* owner;
   bool fetchIsDone;
   const bool isASelect;

   bool isPrepared(Connection&) const noexcept;
   void do_prepare(Connection& connection) throw(adt::RuntimeException, DatabaseException);
   ResultCode do_execute(Connection& connection) throw(adt::RuntimeException, DatabaseException);
   bool do_fetch() throw(adt::RuntimeException, DatabaseException);
   void close() noexcept;

};

} /* namespace sqlite */
} /* namespace dbms */
} /* namespace coffee */

#endif
