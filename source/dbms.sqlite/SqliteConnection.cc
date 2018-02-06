// COFFEE - COmpany eFFEEctive Platform
//
//(c) Copyright 2018 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/coffee
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
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
}

void sqlite::SqliteConnection::close()
   noexcept
{
   if (impl != nullptr) {
      sqlite3_close(impl);
   }
}

void sqlite::SqliteConnection::execute(const char* statement)
   throw(adt::RuntimeException, DatabaseException)
{
   if (impl == nullptr) {
      COFFEE_THROW_EXCEPTION(asString() << " was not open");
   }

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
