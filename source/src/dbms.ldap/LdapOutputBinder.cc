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

#include <coffee/basis/DataBlock.hpp>

#include <coffee/logger/Logger.hpp>

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
#include <coffee/dbms/datatype/MultiString.hpp>

using namespace coffee;
using namespace coffee::dbms;

using coffee::dbms::ldap::LdapOutputBinder;

void LdapOutputBinder::do_decode(Statement& _statement, const int pos)
   throw(basis::RuntimeException)
{
   std::shared_ptr<datatype::Abstract>& data(getData());
   LdapStatement& statement = static_cast<LdapStatement&>(_statement);
   auto handle = statement.getHandle();
   auto entry = statement.getCurrentEntry();

   auto multiValue = coffee_datatype_downcast(datatype::MultiString, data);

   multiValue->clear();

   LOG_DEBUG("Reading " << getData()->getName());

   auto values = ldap_get_values_len(handle, entry, getData()->getName());

   if (values == nullptr) {
      multiValue->isNull();
      return;
   }

   for (int ii = 0, maxii = ldap_count_values_len(values); ii < maxii; ++ ii) {
      multiValue->addValue(values[ii]->bv_val);
   }
   ldap_value_free_len(values);
}


