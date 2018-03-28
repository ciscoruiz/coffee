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

#include <coffee/basis/StreamString.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TraceMethod.hpp>

#include <coffee/dbms/ResultCode.hpp>

#include <coffee/dbms/datatype/Abstract.hpp>
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
   handle(nullptr),
   currentEntry(nullptr)
{
}

LdapStatement::~LdapStatement()
{
   close();
}

ResultCode LdapStatement::do_execute(Connection& connection)
   throw(basis::RuntimeException, DatabaseException)
{
   LOG_THIS_METHOD();

   close();

   handle = static_cast <LdapConnection&>(connection).getImpl();

   const char* _filter = (getExpression().empty()) ? nullptr: getExpression().c_str();

   char** attributes;

   if (input_size() == 0) {
      attributes = new char*[2];
      attributes[0] = (char*)LDAP_ALL_USER_ATTRIBUTES;
      attributes[1] = nullptr;
   }
   else {
      attributes = new char*[input_size() + 1];
      int index = 0;
      for (input_iterator ii = input_begin(), maxii = input_end(); ii != maxii; ++ ii) {
         attributes[index ++] = const_cast <char*> (Statement::data(ii)->getName());
      }
      attributes[index] = nullptr;
   }

   int rc = ldap_search_ext_s(handle, base.c_str (), scope, _filter, attributes, attrOnly, NULL, NULL, NULL, sizeLimit, &result);

   delete []attributes;

   if (rc == LDAP_SUCCESS) {
      if (!ldap_count_entries(handle, result)) {
         ldap_msgfree(result);
         rc = LDAP_NO_SUCH_OBJECT;
      }
   }

   return dbms::ResultCode(connection.getDatabase(), rc);
}

bool LdapStatement::do_fetch()
   throw(basis::RuntimeException, DatabaseException)
{
   if (!result)
      return false;

   if (!currentEntry) {
      currentEntry = ldap_first_entry(handle, result);
   }
   else {
      auto copy = currentEntry;
      currentEntry = ldap_next_entry(handle, copy);
      ldap_memfree(copy);
   }

   LOG_DEBUG("Dn=" << getDn(currentEntry));

   return currentEntry != nullptr;
}

void LdapStatement::close()
   noexcept
{
   if (currentEntry) {
      ldap_memfree(currentEntry);
      currentEntry = nullptr;
   }

   handle = nullptr;
}

std::string LdapStatement::getDn(LDAPMessage* entry)
   noexcept
{
   std::string result("<no-enry>");

   if (entry) {
      char* dn = ldap_get_dn(handle, currentEntry);
      result = dn;
      ldap_memfree(dn);
   }

   return result;
}
