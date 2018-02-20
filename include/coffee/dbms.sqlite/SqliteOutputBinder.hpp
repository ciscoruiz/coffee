#ifndef _coffee_dbms_sqlite_SqliteOutputBinder_hpp_
#define _coffee_dbms_sqlite_SqliteOutputBinder_hpp_

#include <coffee/dbms/binder/Output.hpp>

namespace coffee {
namespace dbms {
namespace sqlite {

class SqliteOutputBinder: public binder::Output {
public:
   explicit SqliteOutputBinder(std::shared_ptr<datatype::Abstract>& value) : binder::Output(value) {;}
   virtual ~SqliteOutputBinder() {;}

private:
   void do_prepare(Statement& statement, const int pos) throw(adt::RuntimeException, DatabaseException) {;}
   void do_decode(Statement& statement, const int pos) throw(adt::RuntimeException);
   void do_write(const std::shared_ptr<datatype::LongBlock>&) throw(adt::RuntimeException, dbms::DatabaseException) {;}
};

} /* namespace sqlite */
} /* namespace dbms */
} /* namespace coffee */

#endif
