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

#ifndef _coffee_dbms_ldap_LdapStatement_hpp_
#define _coffee_dbms_ldap_LdapStatement_hpp_

#include <ldap.h>
#include <sys/time.h>
#include <vector>

#include <coffee/dbms/Statement.hpp>

#include <coffee/dbms.ldap/LdapStatementParameters.hpp>

namespace coffee {
namespace dbms {
namespace ldap {

class LdapInputBinder;

class LdapStatement : public Statement {
public:
   LdapStatement(const Database& database, const char* name, const std::string& expression, const StatementParameters& parameters);
   ~LdapStatement();

private:
   const std::string base;
   const LdapStatementParameters::Scope::_v scope;
   const bool attrOnly;
   timeval timeout;
   const int sizeLimit;
   std::vector<std::string> boundValues;
   LDAP* handle;
   LDAPMessage* result;
   LDAPMessage* currentEntry;

   LDAP* getHandle() noexcept { return handle; }
   LDAPMessage* getCurrentEntry() noexcept { return currentEntry; }
   void setBoundValue(const std::string& boundValue) noexcept { boundValues.push_back(boundValue); }

   void do_prepare(Connection& connection) throw(adt::RuntimeException, DatabaseException) {;}
   ResultCode do_execute(Connection& connection) throw(adt::RuntimeException, DatabaseException);
   bool do_fetch() throw(adt::RuntimeException, DatabaseException);
   void close() noexcept;
   std::string getDn(LDAPMessage* entry) noexcept;

   friend class LdapInputBinder;
   friend class LdapOutputBinder;
};

} /* namespace ldap */
} /* namespace dbms */
} /* namespace coffee */

#endif
