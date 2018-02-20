#ifndef _coffee_dbms_sqlite_SqliteInputBinder_hpp_
#define _coffee_dbms_sqlite_SqliteInputBinder_hpp_

#include <coffee/dbms/binder/Input.hpp>

namespace coffee {
namespace dbms {
namespace sqlite {

class SqliteInputBinder: public binder::Input {
public:
   explicit SqliteInputBinder(std::shared_ptr<datatype::Abstract>& value) : binder::Input(value) {;}
   virtual ~SqliteInputBinder() {;}

private:
   void do_prepare(Statement& statement, const int pos) throw(adt::RuntimeException, DatabaseException) {;}
   void do_encode(Statement& statement, const int pos) throw(adt::RuntimeException, DatabaseException);
};

} /* namespace sqlite */
} /* namespace dbms */
} /* namespace coffee */

#endif
