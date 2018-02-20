#include <coffee/logger/Logger.hpp>

#include <coffee/dbms/Database.hpp>

#include <coffee/dbms.sqlite/SqliteConnection.hpp>

using namespace coffee;
using namespace coffee::dbms;

sqlite::SqliteConnection::SqliteConnection(const Database& database, const std::string& name, const char* user, const char* password) :
   Connection(database, name, user, password),
   impl(nullptr)
{
}

sqlite::SqliteConnection::~SqliteConnection()
{
   close();
}

void sqlite::SqliteConnection::open()
   throw(DatabaseException)
{
   int rc = sqlite3_open(getDatabase().getName().c_str(), &impl);

   if (rc != SQLITE_OK) {
      close();
      ResultCode resultCode(getDatabase(), rc);
      COFFEE_THROW_DB_EXCEPTION(resultCode);
   }
}

void sqlite::SqliteConnection::close()
   noexcept
{
   if (impl != nullptr) {
      sqlite3_close(impl);
      impl = nullptr;
   }
}

void sqlite::SqliteConnection::execute(const char* statement)
   throw(adt::RuntimeException, DatabaseException)
{
   LOG_DEBUG(statement);

   char* errorMessage = 0;

   const int rc = sqlite3_exec(impl, statement, NULL, 0, &errorMessage);

   if (rc != SQLITE_OK) {
      ResultCode resultCode(getDatabase(), rc, errorMessage);
      sqlite3_free(errorMessage);
      COFFEE_THROW_DB_EXCEPTION(resultCode);
   }
}

void sqlite::SqliteConnection::do_rollback()
   noexcept
{
   try {
      execute("ROLLBACK");
   }
   catch (adt::Exception& ex) {
      logger::Logger::write(ex);
   }
}
