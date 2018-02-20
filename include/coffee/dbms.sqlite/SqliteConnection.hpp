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
