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

#include <coffee/adt/DataBlock.hpp>
#include <coffee/adt/Millisecond.hpp>
#include <coffee/adt/Second.hpp>

#include <coffee/dbms.sqlite/SqliteOutputBinder.hpp>
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

void sqlite::SqliteOutputBinder::do_decode(Statement& statement, const int pos)
   throw(adt::RuntimeException)
{
   std::shared_ptr<datatype::Abstract>& data(getData());
   sqlite3_stmt* impl = static_cast<SqliteStatement&>(statement).getImpl();

   if (sqlite3_column_type(impl, pos) == SQLITE_NULL) {
      data->isNull();
      return;
   }

   switch(data->getType()) {
   case dbms::datatype::Abstract::Datatype::Integer:
      coffee_datatype_downcast(datatype::Integer, data)->setValue(sqlite3_column_int(impl, pos));
      break;
   case dbms::datatype::Abstract::Datatype::String:
      coffee_datatype_downcast(datatype::String, data)->setValue(std::string((char*)sqlite3_column_text(impl, pos)));
      break;
   case dbms::datatype::Abstract::Datatype::Float:
      coffee_datatype_downcast(datatype::Float, data)->setValue(sqlite3_column_double(impl, pos));
      break;
   case dbms::datatype::Abstract::Datatype::ShortBlock:
      {
         adt::DataBlock blob((const char*) sqlite3_column_blob(impl, pos), sqlite3_column_bytes(impl, pos));
         coffee_datatype_downcast(datatype::ShortBlock, data)->setValue(blob);
      }
      break;
   case dbms::datatype::Abstract::Datatype::LongBlock:
      {
         adt::DataBlock blob((const char*) sqlite3_column_blob(impl, pos), sqlite3_column_bytes(impl, pos));
         coffee_datatype_downcast(datatype::LongBlock, data)->setValue(blob);
      }
      break;
   case dbms::datatype::Abstract::Datatype::Date:
      {
         adt::Second second(sqlite3_column_int64(impl, pos));
         coffee_datatype_downcast(datatype::Date, data)->setValue(second);
      }
      break;
   case dbms::datatype::Abstract::Datatype::TimeStamp:
      {
         adt::Millisecond msecond(sqlite3_column_int64(impl, pos));
         coffee_datatype_downcast(datatype::TimeStamp, data)->setValue(adt::Second(msecond));
         coffee_datatype_downcast(datatype::TimeStamp, data)->setFractionalSecond(msecond / 1000);
      }
      break;
   }
}


