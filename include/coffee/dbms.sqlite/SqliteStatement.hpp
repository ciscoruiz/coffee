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
