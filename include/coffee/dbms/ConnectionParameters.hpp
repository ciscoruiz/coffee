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
#ifndef _coffee_dbms_ConnectionParameter_h
#define _coffee_dbms_ConnectionParameter_h

#include <coffee/basis/StreamString.hpp>

namespace coffee {

namespace dbms {

class ConnectionParameters {
public:
   ConnectionParameters(const char* user, const char* password) : m_user(user), m_password(password) {;}
   virtual ~ConnectionParameters() { ; }

   const std::string& getUser() const noexcept { return m_user; }
   const std::string& getPassword() const noexcept { return m_password; }

   virtual basis::StreamString asString() const noexcept {
      basis::StreamString result("dbms.ConnectionParameters { ");
      result << "User=" << m_user;
      return result << " }";
   }

private:
   std::string m_user; /**< Nombre del usuario */
   std::string m_password; /**< Clave de acceso del usuario. */
};


}
}


#endif /* _coffee_dbms_ConnectionParameter_h */
