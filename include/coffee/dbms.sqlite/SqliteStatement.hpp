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
