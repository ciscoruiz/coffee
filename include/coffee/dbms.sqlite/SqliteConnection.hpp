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

#ifndef _coffee_dbms_sqlite_SqliteConnection_hpp_
#define _coffee_dbms_sqlite_SqliteConnection_hpp_

#include <sqlite3.h>
#include <coffee/dbms/Connection.hpp>

namespace coffee {
namespace dbms {
namespace sqlite {

class SqliteConnection : public Connection {
public:
   SqliteConnection(const Database& dbmsDatabase, const std::string& name, const char* user, const char* password);
   virtual ~SqliteConnection();

   ::sqlite3* getImpl() { return impl; }

   void execute(const char* statement) throw(adt::RuntimeException, DatabaseException);

private:
   ::sqlite3* impl;

   void open() throw(DatabaseException);
   void close() noexcept;
   bool isAvailable () const noexcept  { return impl != nullptr;  }
   bool do_beginTransaction() throw(adt::RuntimeException, DatabaseException) { execute("BEGIN TRANSACTION;"); return true;}
   void do_commit() throw(adt::RuntimeException, DatabaseException) { execute("COMMIT;"); }
   void do_rollback() noexcept;
};

} /* namespace sqlite */
} /* namespace dbms */
} /* namespace coffee */

#endif
