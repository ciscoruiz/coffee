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

#include <coffee/logger/Logger.hpp>

#include <coffee/dbms/Database.hpp>

#include <coffee/dbms.ldap/LdapConnection.hpp>
#include <coffee/dbms.ldap/LdapConnectionParameters.hpp>

using namespace coffee;
using namespace coffee::dbms;

using coffee::dbms::ldap::LdapConnection;

LdapConnection::LdapConnection(const Database& database, const std::string& name, const ConnectionParameters& parameters) :
   Connection(database, name, parameters),
   protocolVersion(dynamic_cast<const LdapConnectionParameters&>(parameters).getProtocolVersion()),
   useTLS(dynamic_cast<const LdapConnectionParameters&>(parameters).getUseTLS()),
   ldapHandle(nullptr)
{
}

LdapConnection::~LdapConnection()
{
   close();
}

void LdapConnection::open()
   throw(DatabaseException)
{
   checkResult(ldap_initialize(&ldapHandle, getDatabase().getName().c_str()), COFFEE_FILE_LOCATION);

   int version = protocolVersion;
   checkResult(ldap_set_option(ldapHandle, LDAP_OPT_PROTOCOL_VERSION, &version), COFFEE_FILE_LOCATION);

   if (useTLS) {
      int rc = ldap_start_tls_s(ldapHandle, NULL, NULL);

      if (rc == LDAP_CONNECT_ERROR) {
         char* errorMessage;
         ldap_get_option(ldapHandle, LDAP_OPT_DIAGNOSTIC_MESSAGE, (void*)&errorMessage);
         ResultCode resultCode(getDatabase(), rc, errorMessage);
         coffee::dbms::DatabaseException dbex (resultCode, COFFEE_FILE_LOCATION);
         ldap_memfree(errorMessage);
         throw dbex;
      }
      checkResult(rc, COFFEE_FILE_LOCATION);
   }

   BerValue userCrendential;

   userCrendential.bv_val = (char*) getPassword().c_str();
   userCrendential.bv_len = coffee_strlen(userCrendential.bv_val);
   BerValue *serverCredential;

   checkResult(ldap_sasl_bind_s(ldapHandle, getUser().c_str(), LDAP_SASL_SIMPLE, &userCrendential, NULL, NULL, &serverCredential), COFFEE_FILE_LOCATION);
}

void LdapConnection::close()
   noexcept
{
   if (ldapHandle) {
      ldap_unbind_ext_s (ldapHandle, NULL, NULL);
      ldapHandle = nullptr;
   }
}

void LdapConnection::do_rollback()
   noexcept
{
   LOG_WARN("No rollback can be done in LDAP");
}

void LdapConnection::checkResult(const int rc, const char *method, const char* file, const int line)
   throw (DatabaseException)
{
   if (rc != LDAP_SUCCESS) {
      close();
      ResultCode resultCode(getDatabase(), rc, ldap_err2string(rc));
      throw coffee::dbms::DatabaseException (resultCode, method, file, line);
   }
}
