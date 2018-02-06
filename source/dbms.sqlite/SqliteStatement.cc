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
#include <coffee/adt/StreamString.hpp>

#include <coffee/dbms/ResultCode.hpp>

#include <coffee/dbms.sqlite/SqliteStatement.hpp>
#include <coffee/dbms.sqlite/SqliteConnection.hpp>

using namespace coffee;
using namespace coffee::dbms;

sqlite::SqliteStatement::SqliteStatement(const Database& database, const char* name, const std::string& expression, const ActionOnError::_v actionOnError) :
   Statement(database, name, expression, actionOnError),
   impl(nullptr),
   owner(nullptr),
   fetchIsDone(false),
   isASelect(adt::StreamString(expression).toLower().find("select") != std::string::npos)
{
}

sqlite::SqliteStatement::~SqliteStatement()
{
   close();
}

bool sqlite::SqliteStatement::isPrepared(Connection& connection) const
   noexcept
{
   return Statement::isPrepared(connection) and (owner == &connection);
}

void sqlite::SqliteStatement::do_prepare(Connection& connection)
   throw(adt::RuntimeException, DatabaseException)
{
   const char* expression = getExpression().c_str();
   const int len = coffee_strlen(expression);
   sqlite3* dbImpl = static_cast<SqliteConnection&>(connection).getImpl();

   int rc = sqlite3_prepare_v2(dbImpl, expression, len, &impl, NULL);

   if (rc != SQLITE_OK) {
      ResultCode resultCode(connection.getDatabase(), rc, sqlite3_errmsg(dbImpl));
      COFFEE_THROW_DB_EXCEPTION(resultCode);
   }

   const int noParameters = sqlite3_bind_parameter_count(impl) ;

   if (noParameters != input_size()) {
      COFFEE_THROW_EXCEPTION(getExpression() << " number of input binders does not match");
   }

   const int noColumns = sqlite3_column_count(impl) ;

   if (noColumns != output_size()) {
      COFFEE_THROW_EXCEPTION(getExpression() << " number of output binders does not match");
   }
}

ResultCode sqlite::SqliteStatement::do_execute(Connection& connection)
   throw(adt::RuntimeException, DatabaseException)
{
   int rc = sqlite3_reset(impl);

   if (rc != SQLITE_OK) {
      sqlite3* dbImpl = static_cast<SqliteConnection&>(connection).getImpl();
      ResultCode resultCode(connection.getDatabase(), rc, sqlite3_errmsg(dbImpl));
      COFFEE_THROW_DB_EXCEPTION(resultCode);
   }

   rc = sqlite3_step(impl);

   if (isASelect) {
      if (rc == SQLITE_DONE) {
         rc = SQLITE_NOTFOUND;
         fetchIsDone = false;
      }
      else {
         rc = SQLITE_OK;
         fetchIsDone = true;
      }
   }
   else {
      if (rc == SQLITE_DONE)
         rc = SQLITE_OK;
   }

   return dbms::ResultCode(connection.getDatabase(), rc);
}

bool sqlite::SqliteStatement::do_fetch()
   throw(adt::RuntimeException, DatabaseException)
{
   if (fetchIsDone) {
      fetchIsDone = false;
      return true;
   }
   return sqlite3_step(impl) == SQLITE_ROW;
}

void sqlite::SqliteStatement::close()
   noexcept
{
   if(impl) {
      sqlite3_finalize(impl);
      impl = nullptr;
   }
}
