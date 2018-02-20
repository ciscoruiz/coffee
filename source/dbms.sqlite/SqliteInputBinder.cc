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

#include <coffee/adt/Millisecond.hpp>

#include <coffee/dbms.sqlite/SqliteInputBinder.hpp>
#include <coffee/dbms.sqlite/SqliteConnection.hpp>
#include <coffee/dbms.sqlite/SqliteStatement.hpp>

#include <sqlite3.h>

#include <coffee/dbms/ResultCode.hpp>
#include <coffee/dbms/datatype/Integer.hpp>
#include <coffee/dbms/datatype/String.hpp>
#include <coffee/dbms/datatype/Float.hpp>
#include <coffee/dbms/datatype/ShortBlock.hpp>
#include <coffee/dbms/datatype/LongBlock.hpp>
#include <coffee/dbms/datatype/Date.hpp>
#include <coffee/dbms/datatype/TimeStamp.hpp>

using namespace coffee;
using namespace coffee::dbms;

void sqlite::SqliteInputBinder::do_encode(Statement& statement, const int pos)
   throw(adt::RuntimeException, DatabaseException)
{
   std::shared_ptr<datatype::Abstract>& data(getData());
   sqlite3_stmt* impl = static_cast<SqliteStatement&>(statement).getImpl();

   int rc;

   if (data->hasValue() == false) {
      rc = sqlite3_bind_null(impl, pos + 1);
   }
   else {
      switch(data->getType()) {
      case dbms::datatype::Abstract::Datatype::Integer:
         rc = sqlite3_bind_int(impl, pos + 1, coffee_datatype_downcast(datatype::Integer, data)->getValue());
         break;
      case dbms::datatype::Abstract::Datatype::String:
         {
            const char* text = coffee_datatype_downcast(datatype::String, data)->getValue();
            rc = sqlite3_bind_text(impl, pos + 1, text, coffee_strlen(text), SQLITE_TRANSIENT);
         }
         break;
      case dbms::datatype::Abstract::Datatype::Float:
         rc = sqlite3_bind_double(impl, pos + 1, coffee_datatype_downcast(datatype::Float, data)->getValue());
         break;
      case dbms::datatype::Abstract::Datatype::ShortBlock:
         {
            auto shortBlock = coffee_datatype_downcast(datatype::ShortBlock, data)->getValue();
            rc = sqlite3_bind_blob(impl, pos + 1, (void*)shortBlock.data(), shortBlock.size(), SQLITE_TRANSIENT);
         }
         break;
      case dbms::datatype::Abstract::Datatype::LongBlock:
         {
            auto longBlock = coffee_datatype_downcast(datatype::LongBlock, data)->getValue();
            rc = sqlite3_bind_blob(impl, pos + 1, (void*)longBlock.data(), longBlock.size(), SQLITE_TRANSIENT);
         }
         break;
      case dbms::datatype::Abstract::Datatype::Date:
         {
            auto date = coffee_datatype_downcast(datatype::Date, data)->getValue();
            const std::string text = date.asDateTime("%Y-%m-%d %H:%M:%S");
            rc = sqlite3_bind_text(impl, pos + 1, text.c_str(), text.length(), SQLITE_TRANSIENT);
         }
         break;
      case dbms::datatype::Abstract::Datatype::TimeStamp:
         {
            auto timeStamp = coffee_datatype_downcast(datatype::TimeStamp, data);
            adt::Millisecond ms = timeStamp->getValue().getTime();
            ms += adt::Millisecond(timeStamp->getFractionalSecond());
            rc = sqlite3_bind_int64(impl, pos + 1, ms.getValue());
         }
         break;
      }
   }

   if (rc != SQLITE_OK) {
      Connection* owner = static_cast<SqliteStatement&>(statement).getOwner();
      sqlite3* dbImpl = static_cast<SqliteConnection*>(owner)->getImpl();
      ResultCode resultCode(statement.getDatabase(), rc, sqlite3_errmsg(dbImpl));
      COFFEE_THROW_DB_EXCEPTION(resultCode);
   }
}


