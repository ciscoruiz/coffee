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
