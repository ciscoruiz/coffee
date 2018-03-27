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

#include <coffee/basis/AsString.hpp>

#include <coffee/dbms.ldap/LdapInputBinder.hpp>
#include <coffee/dbms.ldap/LdapConnection.hpp>
#include <coffee/dbms.ldap/LdapStatement.hpp>

#include <ldap.h>

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

using coffee::dbms::ldap::LdapInputBinder;

void LdapInputBinder::do_encode(Statement& _statement, const int pos)
   throw(basis::RuntimeException, DatabaseException)
{
   static const std::string empty;

   std::shared_ptr<datatype::Abstract>& data(getData());
   LdapStatement& statement = static_cast<LdapStatement&>(_statement);

   int rc = LDAP_SUCCESS;

   if (data->hasValue() == false) {
      statement.setBoundValue(empty);
   }
   else {
      switch(data->getType()) {
      case dbms::datatype::Abstract::Datatype::Integer:
         statement.setBoundValue(basis::AsString::apply(coffee_datatype_downcast(datatype::Integer, data)->getValue()));
         break;
      case dbms::datatype::Abstract::Datatype::String:
         statement.setBoundValue(basis::AsString::apply(coffee_datatype_downcast(datatype::String, data)->getValue()));
         break;
      case dbms::datatype::Abstract::Datatype::Float:
         statement.setBoundValue(basis::AsString::apply(coffee_datatype_downcast(datatype::Float, data)->getValue()));
         break;
      case dbms::datatype::Abstract::Datatype::ShortBlock:
      case dbms::datatype::Abstract::Datatype::LongBlock:
         rc = LDAP_NOT_SUPPORTED;
         break;
      case dbms::datatype::Abstract::Datatype::Date:
         {
            auto date = coffee_datatype_downcast(datatype::Date, data)->getValue();
            statement.setBoundValue(basis::AsString::apply(date, datatype::Date::DefaultFormat));
         }
         break;
      case dbms::datatype::Abstract::Datatype::TimeStamp:
         {
            const std::chrono::seconds& timeStamp = coffee_datatype_downcast(datatype::TimeStamp, data)->getValue();
            statement.setBoundValue(basis::AsString::apply(timeStamp));
         }
         break;
      }
   }

   if (rc != LDAP_SUCCESS) {
      ResultCode resultCode(statement.getDatabase(), rc, ldap_err2string(rc));
      COFFEE_THROW_DB_EXCEPTION(resultCode);
   }
}

