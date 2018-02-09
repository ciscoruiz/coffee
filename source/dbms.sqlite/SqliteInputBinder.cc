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
            rc = sqlite3_bind_text(impl, pos + 1, text, coffee_strlen(text), NULL);
         }
         break;
      case dbms::datatype::Abstract::Datatype::Float:
         rc = sqlite3_bind_double(impl, pos + 1, coffee_datatype_downcast(datatype::Float, data)->getValue());
         break;
      case dbms::datatype::Abstract::Datatype::ShortBlock:
         {
            auto shortBlock = coffee_datatype_downcast(datatype::ShortBlock, data)->getValue();
            rc = sqlite3_bind_blob(impl, pos + 1, (void*)shortBlock.data(), shortBlock.size(), NULL);
         }
         break;
      case dbms::datatype::Abstract::Datatype::LongBlock:
         {
            auto longBlock = coffee_datatype_downcast(datatype::LongBlock, data)->getValue();
            rc = sqlite3_bind_blob64(impl, pos + 1, (void*)longBlock.data(), longBlock.size(), NULL);
         }
         break;
      case dbms::datatype::Abstract::Datatype::Date:
         {
            auto date = coffee_datatype_downcast(datatype::Date, data)->getValue();
            rc = sqlite3_bind_int64(impl, pos + 1, date.getTime().getValue());
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

