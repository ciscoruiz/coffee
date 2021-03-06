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

#include <gtest/gtest.h>

#include <coffee/config/defines.hpp>
#include <coffee/dbms.ldap/LdapStatementParameters.hpp>

using coffee::dbms::ldap::LdapStatementParameters;

TEST(StatementParameterTest, sspp_scope)
{
   LdapStatementParameters parameters(coffee::dbms::ActionOnError::Ignore, "my-dn");

   parameters.setSizeLimit(100).setScope(LdapStatementParameters::Scope::Base);
   ASSERT_EQ(LDAP_SCOPE_BASE, parameters.getScope());
   ASSERT_EQ(100, parameters.getSizeLimit());

   parameters.setSizeLimit(100).setScope(LdapStatementParameters::Scope::OneLevel);
   ASSERT_EQ(LDAP_SCOPE_ONELEVEL, parameters.getScope());

   parameters.setSizeLimit(100).setScope(LdapStatementParameters::Scope::SubTree);
   ASSERT_EQ(LDAP_SCOPE_SUBTREE, parameters.getScope());
}

TEST(StatementParameterTest, sspp_dn)
{
   LdapStatementParameters parameters(coffee::dbms::ActionOnError::Ignore, "my-dn");
   ASSERT_EQ("my-dn", parameters.getBaseDN());
}

TEST(StatementParameterTest, sspp_attronly)
{
   LdapStatementParameters parameters(coffee::dbms::ActionOnError::Ignore, "my-dn");

   ASSERT_TRUE(!parameters.getAttrOnly());
   parameters.setScope(LdapStatementParameters::Scope::Base).setAttrOnly(true);
   ASSERT_EQ(LDAP_SCOPE_BASE, parameters.getScope());
   ASSERT_TRUE(parameters.getAttrOnly());
}

TEST(StatementParameterTest, sspp_asstring)
{
   LdapStatementParameters parameters(coffee::dbms::ActionOnError::Ignore, "my-dn");

   ASSERT_EQ("dbms.ldap.LdapStatementParameters {dbms.StatementParameters { ActionOnError=Ignore } | BaseDN=my-dn | Scope=Base | AttrOnly=false | SizeLimit=0 }", parameters.asString());

   parameters.setSizeLimit(200).setScope(LdapStatementParameters::Scope::OneLevel).setAttrOnly(true);
   ASSERT_EQ("dbms.ldap.LdapStatementParameters {dbms.StatementParameters { ActionOnError=Ignore } | BaseDN=my-dn | Scope=OneLevel | AttrOnly=true | SizeLimit=200 }", parameters.asString());

   parameters.setScope(LdapStatementParameters::Scope::SubTree);
   ASSERT_EQ("dbms.ldap.LdapStatementParameters {dbms.StatementParameters { ActionOnError=Ignore } | BaseDN=my-dn | Scope=SubTree | AttrOnly=true | SizeLimit=200 }", parameters.asString());
}
