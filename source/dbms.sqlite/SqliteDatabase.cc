#include <memory>

#include <coffee/dbms.sqlite/SqliteDatabase.hpp>
#include <coffee/dbms.sqlite/SqliteConnection.hpp>
#include <coffee/dbms.sqlite/SqliteStatement.hpp>
#include <coffee/dbms.sqlite/SqliteInputBinder.hpp>
#include <coffee/dbms.sqlite/SqliteOutputBinder.hpp>
#include <coffee/dbms.sqlite/SqliteErrorCodeInterpreter.hpp>

using namespace coffee;
using namespace coffee::dbms;

sqlite::SqliteDatabase::SqliteDatabase(const boost::filesystem::path& dbFile) :
   Database("SQLite3", dbFile.c_str())
{
   setErrorCodeInterpreter(std::make_shared<sqlite::SqliteErrorCodeInterpreter>());
}

sqlite::SqliteDatabase::~SqliteDatabase()
{
}

std::shared_ptr<Connection> sqlite::SqliteDatabase::allocateConnection(const std::string& name, const char* user, const char* password)
   throw(adt::RuntimeException)
{
   return std::make_shared<sqlite::SqliteConnection>(*this, name, user, password);
}

std::shared_ptr<Statement> sqlite::SqliteDatabase::allocateStatement(const char* name, const std::string& expression, const ActionOnError::_v actionOnError)
   throw(adt::RuntimeException)
{
   return std::make_shared<sqlite::SqliteStatement>(*this, name, expression, actionOnError);
}

std::shared_ptr<binder::Input> sqlite::SqliteDatabase::allocateInputBind(std::shared_ptr<datatype::Abstract> data) const
   throw(adt::RuntimeException)
{
   return std::make_shared<sqlite::SqliteInputBinder>(data);
}

std::shared_ptr<binder::Output> sqlite::SqliteDatabase::allocateOutputBind(std::shared_ptr<datatype::Abstract> data) const
   throw(adt::RuntimeException)
{
   return std::make_shared<sqlite::SqliteOutputBinder>(data);
}

