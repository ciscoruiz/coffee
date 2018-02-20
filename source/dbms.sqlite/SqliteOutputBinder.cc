
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


