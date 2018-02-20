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
