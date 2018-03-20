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

#include <coffee/dbms.ldap/LdapStatement.hpp>
#include <coffee/dbms.ldap/LdapConnection.hpp>

using namespace coffee;
using namespace coffee::dbms;

using coffee::dbms::ldap::LdapStatement;
using coffee::dbms::ldap::LdapStatementParameters;

LdapStatement::LdapStatement(const Database& database, const char* name, const std::string& expression, const StatementParameters& parameters) :
   Statement(database, name, expression, parameters),
   base(dynamic_cast<const LdapStatementParameters&>(parameters).getBaseDN()),
   scope(dynamic_cast<const LdapStatementParameters&>(parameters).getScope()),
   attrOnly(dynamic_cast<const LdapStatementParameters&>(parameters).getAttrOnly()),
   sizeLimit(dynamic_cast<const LdapStatementParameters&>(parameters).getSizeLimit()),
   result(nullptr),
   handle(nullptr)
{
}

LdapStatement::~LdapStatement()
{
   close();
}

ResultCode LdapStatement::do_execute(Connection& connection)
   throw(adt::RuntimeException, DatabaseException)
{
   close();

   handle = static_cast <LdapConnection&>(connection).getImpl();

   if (boundValues.size() == 0) {
      COFFEE_THROW_EXCEPTION(asString() << " does not have any bound value");
   }

   const char* _filter = (filter.empty()) ? nullptr: filter.c_str();
   char** attributes = new char*[boundValues.size()];
   int index = 0;
   for (auto ii = boundValues.begin(), maxii = boundValues.end(); ii != maxii; ++ ii, ++ index) {
      attributes[index] =  const_cast <char*> (ii->c_str());
   }
   attributes[index] = nullptr;

   int rc = ldap_search_ext_s(handle, base.c_str (), scope, _filter, attributes, attrOnly, NULL, NULL, &timeout, sizeLimit, &result);

   delete []attributes;

   if (rc == LDAP_SUCCESS) {
      if (!ldap_count_entries(handle, result) == 0) {
         ldap_msgfree(result);
         rc = LDAP_NO_SUCH_OBJECT;
      }
      else {
         pos = ldap_first_entry(handle, result);
      }
   }

   return dbms::ResultCode(connection.getDatabase(), rc);
}

bool LdapStatement::do_fetch()
   throw(adt::RuntimeException, DatabaseException)
{
   auto copy = pos;
   pos = ldap_next_entry(handle, copy);
   ldap_memfree(copy);
   return pos != nullptr;
}

void LdapStatement::close()
   noexcept
{
   handle = nullptr;

   if(result) {
      ldap_msgfree(result);
      result = nullptr;
   }

   if (pos) {
      ldap_msgfree(pos);
      pos = nullptr;
   }
}
