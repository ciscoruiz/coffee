#ifndef _coffee_dbms_sqlite_SqliteDatabase_hpp_
#define _coffee_dbms_sqlite_SqliteDatabase_hpp_

#include <boost/filesystem.hpp>

#include <coffee/dbms/Database.hpp>
#include <coffee/dbms/ErrorCodeInterpreter.hpp>

namespace coffee {

namespace dbms  {

namespace sqlite {

class SqliteDatabase : public Database {
public:
   explicit SqliteDatabase(const boost::filesystem::path& dbFile);
   ~SqliteDatabase();

private:
   class SqliteErrorCodeInterpreter : public dbms::ErrorCodeInterpreter {
   public:
      SqliteErrorCodeInterpreter() {;}
   private:
      bool notFound(const int errorCode) const throw(adt::RuntimeException);
      bool successful(const int errorCode) const throw(adt::RuntimeException);
      bool locked(const int errorCode) const throw(adt::RuntimeException);
      bool lostConnection(const int errorCode) const throw(adt::RuntimeException);
   };

   std::shared_ptr<Connection> allocateConnection(const std::string& name, const char* user, const char* password)
      throw(adt::RuntimeException);

   std::shared_ptr<Statement> allocateStatement(const char* name, const std::string& expression, const ActionOnError::_v actionOnError)
      throw(adt::RuntimeException);

   std::shared_ptr<binder::Input> allocateInputBind(std::shared_ptr<datatype::Abstract> data) const
      throw(adt::RuntimeException);
   std::shared_ptr<binder::Output> allocateOutputBind(std::shared_ptr<datatype::Abstract> data) const
      throw(adt::RuntimeException);

};

}
}
}

#endif
