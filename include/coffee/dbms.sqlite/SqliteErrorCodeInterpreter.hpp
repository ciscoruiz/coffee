#ifndef _coffee_dbms_sqlite_SqliteErrorCodeInterpreter_hpp_
#define _coffee_dbms_sqlite_SqliteErrorCodeInterpreter_hpp_

#include <coffee/dbms/ErrorCodeInterpreter.hpp>

namespace coffee {
namespace dbms {
namespace sqlite {

class SqliteErrorCodeInterpreter : public ErrorCodeInterpreter {
public:
   SqliteErrorCodeInterpreter() {;}

private:
   bool notFound(const int errorCode) const throw(adt::RuntimeException);
   bool successful(const int errorCode) const throw(adt::RuntimeException);
   bool locked(const int errorCode) const throw(adt::RuntimeException);
   bool lostConnection(const int errorCode) const throw(adt::RuntimeException);
};

} /* namespace sqlite */
} /* namespace dbms */
} /* namespace coffee */

#endif
