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
#ifndef _coffee_dbms_ldap_LdapStatementParameters_h
#define _coffee_dbms_ldap_LdapStatementParameters_h

#include <ldap.h>

#include <coffee/dbms/StatementParameters.hpp>

namespace coffee {
namespace dbms {
namespace ldap {

class LdapStatementParameters : public StatementParameters {
public:
   struct Scope {
      enum _v {
         Base = LDAP_SCOPE_BASE, /**< It will only search on the level indicates by DN */
         OneLevel = LDAP_SCOPE_ONELEVEL, /**< It will search on the level indicates by DN and its direct dependencies  */
         SubTree  = LDAP_SCOPE_SUBTREE /**< It will search on the level indicates by DN and its all its dependencies */
      };
   };

   LdapStatementParameters(const ActionOnError::_v actionOnError, const std::string& _baseDN) :
      StatementParameters(actionOnError),
      baseDN(_baseDN),
      scope(Scope::Base),
      attrOnly(false),
      sizeLimit(LDAP_NO_LIMIT)
   {}

   LdapStatementParameters& setScope(const Scope::_v scope) noexcept { this->scope = scope; return *this; }
   LdapStatementParameters& setAttrOnly(const bool attrOnly) noexcept { this->attrOnly = attrOnly; return *this; }
   LdapStatementParameters& setSizeLimit(const int sizeLimit) noexcept { this->sizeLimit = sizeLimit; return *this; }

   const std::string& getBaseDN() const noexcept { return baseDN; }
   Scope::_v getScope() const noexcept { return scope; }
   bool getAttrOnly() const noexcept { return attrOnly; }
   int getSizeLimit() const noexcept { return sizeLimit; }

   adt::StreamString asString() const noexcept {
      adt::StreamString result("dbms.ldap.LdapStatementParameters {");
      result << StatementParameters::asString();
      result << " | BaseDN=" << baseDN;
      result << " | Scope=" << (scope == Scope::Base ? "Base": (scope == Scope::OneLevel ? "OneLevel": "SubTree"));
      result << " | AttrOnly=" << attrOnly;
      result << " | SizeLimit=" << sizeLimit;
      return result << " }";
   }

private:
   std::string baseDN;
   Scope::_v scope;
   bool attrOnly;
   int sizeLimit;
};

}
}
}

#endif /* _coffee_dbms_ldap_LdapStatementParameters_h */
