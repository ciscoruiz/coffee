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

#include <memory>

#include <coffee/app/Application.hpp>

#include <coffee/dbms.ldap/LdapDatabase.hpp>
#include <coffee/dbms.ldap/LdapConnection.hpp>
#include <coffee/dbms.ldap/LdapStatement.hpp>
#include <coffee/dbms.ldap/LdapInputBinder.hpp>
#include <coffee/dbms.ldap/LdapOutputBinder.hpp>
#include <coffee/dbms.ldap/LdapErrorCodeInterpreter.hpp>

#include <coffee/dbms.ldap/SCCS.hpp>

using namespace coffee;
using namespace coffee::dbms;

using coffee::dbms::ldap::LdapDatabase;

//static
const std::string LdapDatabase::Implementation("ldap");

//static
std::shared_ptr<LdapDatabase> LdapDatabase::instantiate(app::Application& application, const std::string& url)
   throw(basis::RuntimeException)
{
   std::shared_ptr<LdapDatabase> result(new LdapDatabase(application, url));
   application.attach(result);
   return result;
}

LdapDatabase::LdapDatabase(app::Application& application, const std::string& url) :
   Database(application, Implementation, url.c_str())
{
   ldap::SCCS::activate();

   setErrorCodeInterpreter(std::make_shared<ldap::LdapErrorCodeInterpreter>());
}

LdapDatabase::~LdapDatabase()
{
}

std::shared_ptr<Connection> LdapDatabase::allocateConnection(const std::string& name, const ConnectionParameters& parameters)
   throw(basis::RuntimeException, std::bad_cast)
{
   return std::make_shared<LdapConnection>(*this, name, parameters);
}

std::shared_ptr<Statement> LdapDatabase::allocateStatement(const char* name, const std::string& expression, const StatementParameters& parameters)
   throw(basis::RuntimeException, std::bad_cast)
{
   return std::make_shared<LdapStatement>(*this, name, expression, parameters);
}

std::shared_ptr<binder::Input> LdapDatabase::allocateInputBind(std::shared_ptr<datatype::Abstract> data) const
   throw(basis::RuntimeException)
{
   return std::make_shared<LdapInputBinder>(data);
}

std::shared_ptr<binder::Output> LdapDatabase::allocateOutputBind(std::shared_ptr<datatype::Abstract> data) const
   throw(basis::RuntimeException)
{
   return std::make_shared<LdapOutputBinder>(data);
}

