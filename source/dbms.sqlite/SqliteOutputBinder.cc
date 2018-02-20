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
         const std::string date = (const char*) sqlite3_column_text(impl, pos);
         coffee_datatype_downcast(datatype::Date, data)->setValue(adt::Second::fromString(date, "%Y-%m-%d %H:%M:%S"));
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


