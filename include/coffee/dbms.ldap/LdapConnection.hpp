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

#ifndef _coffee_dbms_ldap_LdapConnection_hpp_
#define _coffee_dbms_ldap_LdapConnection_hpp_

#include <ldap.h>

#include <coffee/dbms/Connection.hpp>

namespace coffee {
namespace dbms {
namespace ldap {

class LdapConnection : public Connection {
public:
   LdapConnection(const Database& dbmsDatabase, const std::string& name, const ConnectionParameters& parameters);
   ~LdapConnection();

   LDAP* getImpl() { return impl; }

private:
   const char* dn;
   const int protocolVersion;

   LDAP* impl;

   void open() throw(DatabaseException);
   void close() noexcept;
   bool isAvailable () const noexcept  { return impl != nullptr;  }
   void do_commit() throw(adt::RuntimeException, DatabaseException) { ; }
   void do_rollback() noexcept;

   void checkResult(const int rc) throw (DatabaseException);
};

} /* namespace ldap */
} /* namespace dbms */
} /* namespace coffee */

#endif
