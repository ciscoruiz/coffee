#include <sqlite3.h>

#include <coffee/dbms.sqlite/SqliteErrorCodeInterpreter.hpp>

using namespace coffee;
using namespace coffee::dbms;

bool sqlite::SqliteErrorCodeInterpreter::notFound(const int errorCode) const
   throw(adt::RuntimeException)
{
   return errorCode == SQLITE_NOTFOUND;
}

bool sqlite::SqliteErrorCodeInterpreter::successful(const int errorCode) const
   throw(adt::RuntimeException)
{
   return errorCode == SQLITE_OK;
}

bool sqlite::SqliteErrorCodeInterpreter::locked(const int errorCode) const
   throw(adt::RuntimeException)
{
   return errorCode == SQLITE_LOCKED;
}

bool sqlite::SqliteErrorCodeInterpreter::lostConnection(const int errorCode) const
   throw(adt::RuntimeException)
{
   return errorCode == SQLITE_NOTADB || errorCode == SQLITE_CORRUPT;
}

