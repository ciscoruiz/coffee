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

#include <boost/test/unit_test.hpp>

#include <coffee/config/defines.hpp>
#include <coffee/dbms.ldap/LdapConnectionParameters.hpp>

using coffee::dbms::ldap::LdapConnectionParameters;

BOOST_AUTO_TEST_CASE(ldap_ccpp_version)
{
   LdapConnectionParameters parameters("0", "0");
   parameters.setVersion(LdapConnectionParameters::ProtocolVersion::V2);
   BOOST_REQUIRE_EQUAL(parameters.getProtocolVersion(),LdapConnectionParameters::ProtocolVersion::V2);
}

BOOST_AUTO_TEST_CASE(ldap_ccpp_tls)
{
   LdapConnectionParameters parameters("0", "0");

   parameters.setVersion(LdapConnectionParameters::ProtocolVersion::V3).setUseTLS(true);
   BOOST_REQUIRE(parameters.getUseTLS());
   BOOST_REQUIRE_EQUAL(parameters.getProtocolVersion(),LdapConnectionParameters::ProtocolVersion::V3);

   parameters.setUseTLS(false);
   BOOST_REQUIRE(!parameters.getUseTLS());
}

BOOST_AUTO_TEST_CASE(ldap_ccpp_asString)
{
   LdapConnectionParameters parameters("uid=tesla,dc=example,dc=com", "----");

   parameters.setVersion(LdapConnectionParameters::ProtocolVersion::V3).setUseTLS(true);
   BOOST_REQUIRE_EQUAL(parameters.asString(), "dbms.LdapConnectionParameters { dbms.ConnectionParameters { User=uid=tesla,dc=example,dc=com } | ProtocolVersion=V3(3) | UseTLS=true }");
}

