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

#include <inttypes.h>
#include <stdio.h>

#include <coffee/adt/DataBlock.hpp>

#include <coffee/dbms.ldap/LdapOutputBinder.hpp>
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

using coffee::dbms::ldap::LdapOutputBinder;

void LdapOutputBinder::do_decode(Statement& _statement, const int pos)
   throw(adt::RuntimeException)
{
   std::shared_ptr<datatype::Abstract>& data(getData());
   LdapStatement& statement = static_cast<LdapStatement&>(_statement);
   auto handle = statement.getHandle();
   auto entry = statement.getMessage();
   BerElement* ber;

   auto attribute = ldap_first_attribute(handle, entry, &ber);

   if (attribute == nullptr) {
      data->isNull();
      return;
   }

   auto values = ldap_get_values_len(handle, entry, attribute);

   const int maxValues = ldap_count_values_len(values);

   if (maxValues <= pos) {
      ldap_value_free_len(values);
      COFFEE_THROW_EXCEPTION("Position=" << pos << " is out or range for MaxRange=" << maxValues);
   }

   const char* value = values[pos]->bv_val;

   switch(data->getType()) {
   case dbms::datatype::Abstract::Datatype::Integer:
      coffee_datatype_downcast(datatype::Integer, data)->setValue(atoi(value));
      break;
   case dbms::datatype::Abstract::Datatype::String:
      coffee_datatype_downcast(datatype::String, data)->setValue(value);
      break;
   case dbms::datatype::Abstract::Datatype::Float:
      coffee_datatype_downcast(datatype::Float, data)->setValue(atof(value));
      break;
   case dbms::datatype::Abstract::Datatype::ShortBlock:
   case dbms::datatype::Abstract::Datatype::LongBlock:
      break;
   case dbms::datatype::Abstract::Datatype::Date:
      {
         coffee_datatype_downcast(datatype::Date, data)->setValue(value, datatype::Date::DefaultFormat);
      }
      break;
   case dbms::datatype::Abstract::Datatype::TimeStamp:
      {
         int64_t i64;
         sscanf(value, "%" SCNd64, &i64);
         std::chrono::seconds value(i64);
         coffee_datatype_downcast(datatype::TimeStamp, data)->setValue(value);
      }
      break;
   }

   ldap_value_free_len(values);
}
