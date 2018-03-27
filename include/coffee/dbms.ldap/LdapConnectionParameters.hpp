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
#ifndef _coffee_dbms_ldap_LdapConnectionParameters_h
#define _coffee_dbms_ldap_LdapConnectionParameters_h

#include <ldap.h>

#include <coffee/dbms/ConnectionParameters.hpp>

namespace coffee {
namespace dbms {
namespace ldap {

class LdapConnectionParameters : public ConnectionParameters {
public:
   struct ProtocolVersion {
      enum _v {
         V2 = LDAP_VERSION2,
         V3 = LDAP_VERSION3
      };
   };

   LdapConnectionParameters(const char* user, const char* password) :
      ConnectionParameters(user, password),
      protocolVersion(ProtocolVersion::V3),
      useTLS(false)
   {}

   LdapConnectionParameters& setVersion(const ProtocolVersion::_v version) noexcept { this->protocolVersion = version; return *this; }
   LdapConnectionParameters& setUseTLS(const bool value) noexcept { useTLS = value; return *this; }

   ProtocolVersion::_v getProtocolVersion() const noexcept { return protocolVersion; }
   bool getUseTLS() const noexcept { return useTLS; }

   virtual adt::StreamString asString() const noexcept {
      adt::StreamString result("dbms.LdapConnectionParameters { ");
      result << ConnectionParameters::asString();
      result << " | ProtocolVersion=" << (protocolVersion == ProtocolVersion::V2 ? "V2": "V3") << "(" << (int) protocolVersion << ")";
      result << " | UseTLS=" << useTLS;
      return result << " }";
   }

private:
   ProtocolVersion::_v protocolVersion;
   bool useTLS;
};

}
}
}

#endif /* _coffee_dbms_ldap_LdapConnectionParameters_h */
