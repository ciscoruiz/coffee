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

#include <stdlib.h>
#include <locale.h>

#include <string>
#include <algorithm>

#include <coffee/config/defines.hpp>

#include <coffee/logger/TraceMethod.hpp>
#include <coffee/logger/Logger.hpp>

#include <coffee/xml/Node.hpp>
#include <coffee/xml/Attribute.hpp>

#include <coffee/dbms/Database.hpp>
#include <coffee/dbms/Statement.hpp>
#include <coffee/dbms/Connection.hpp>
#include <coffee/dbms/ConnectionParameters.hpp>
#include <coffee/dbms/FailRecoveryHandler.hpp>
#include <coffee/dbms/StatementTranslator.hpp>
#include <coffee/dbms/SCCS.hpp>


using namespace std;
using namespace coffee;

//static
dbms::StatementParameters dbms::Database::defaultParameters(dbms::ActionOnError::Rollback);

dbms::Database::Database(app::Application& app, const std::string& implementation, const char* dbmsName) :
   app::Service(app, app::Feature::Database, implementation),
   m_name((dbmsName == NULL) ? "local": dbmsName),
   m_failRecoveryHandler(NULL),
   m_statementTranslator(NULL)
{
   dbms::SCCS::activate();
}

dbms::Database::~Database()
{
}

void dbms::Database::do_initialize()
   throw(basis::RuntimeException)
{
   LOG_THIS_METHOD();

   int counter(0);
   bool error = false;

   for(connection_iterator iic = connection_begin(), maxiic = connection_end(); iic != maxiic; iic ++) {
      try {
         connection(iic)->open();
         counter ++;
      }
      catch(basis::Exception& ex) {
         logger::Logger::write(ex);
         error = true;
      }
   }

   if(counter == 0 && error == true) {
      COFFEE_THROW_EXCEPTION(asString() << " | There is not connection available");
   }

   LOG_INFO(asString() << " | It is initialized");
}

void dbms::Database::do_stop()
   throw(basis::RuntimeException)
{
   LOG_THIS_METHOD();

   int counter = 0;

   for(connection_iterator iic = connection_begin(), maxiic = connection_end(); iic != maxiic; ++ iic) {
      std::shared_ptr<Connection>& _connection = connection(iic);
      try {
         _connection->close();
         ++ counter;
      }
      catch(basis::Exception& ex) {
         logger::Logger::write(ex);
      }
   }

   LOG_DEBUG(asString() << " | Closed " << counter << " of " << m_connections.size());

   m_connections.clear();
   m_statements.clear();
}

std::shared_ptr<dbms::Connection> dbms::Database::createConnection(const char* name, const ConnectionParameters& parameters)
   throw(basis::RuntimeException, dbms::DatabaseException)
{
   logger::TraceMethod ttmm(logger::Level::Local7, COFFEE_FILE_LOCATION);

   LOG_DEBUG("Name=" << name << " | User=" << parameters.getUser());

   if(m_connections.size() >= MaxConnection) {
      COFFEE_THROW_EXCEPTION(asString() << " Can not create more than " << MaxConnection);
   }

   for(connection_iterator ii = connection_begin(), maxii = connection_end(); ii != maxii; ii ++) {
      if(connection(ii)->getName() == name) {
         COFFEE_THROW_EXCEPTION(asString() << " | Connection=" << name << " is already defined");
      }
   }

   string strname(name);

   std::shared_ptr<Connection> result;

   try {
      result = allocateConnection(strname, parameters);

      LOG_DEBUG(result->asString());

      if(this->isRunning() == true) {
         try {
            result->open();
            m_connections.push_back(result);
         }
         catch(basis::Exception& ex) {
            logger::Logger::write(ex);
            result.reset();
            throw;
         }
      }
      else
         m_connections.push_back(result);
   }
   catch(std::bad_cast& ex) {
      throw basis::RuntimeException(ex.what(), COFFEE_FILE_LOCATION);
   }

   LOG_DEBUG(result->asString());

   return result;
}

std::shared_ptr<dbms::Connection>& dbms::Database::findConnection(const char* name)
   throw(basis::RuntimeException)
{
   logger::TraceMethod ttmm(logger::Level::Local7, COFFEE_FILE_LOCATION);

   LOG_DEBUG("Name=" << name);

   for(connection_iterator ii = connection_begin(), maxii = connection_end(); ii != maxii; ii ++) {
      if(connection(ii)->getName() == name) {
         return connection(ii);
      }
   }

   COFFEE_THROW_EXCEPTION(asString() << " | Connection='" << name << "' is not defined");
}

std::shared_ptr<dbms::Statement> dbms::Database::createStatement(const char* name, const char* expression, const StatementParameters& parameters)
   throw(basis::RuntimeException)
{
   logger::TraceMethod ttmm(logger::Level::Local7, COFFEE_FILE_LOCATION);

   for(statement_iterator ii = statement_begin(), maxii = statement_end(); ii != maxii; ++ ii) {
      if(statement(ii)->getName() == name)
         COFFEE_THROW_EXCEPTION("Sentence '" << name << "' is already defined");
   }

   if(m_statementTranslator)
      expression = m_statementTranslator->apply(expression);

   std::shared_ptr<Statement> result;

   try {
      result = allocateStatement(name, expression, parameters);
      LOG_DEBUG(result->asString());
      m_statements.push_back(result);
   }
   catch(std::bad_cast& ex) {
      throw basis::RuntimeException(ex.what(), COFFEE_FILE_LOCATION);
   }

   return result;
}

std::shared_ptr<dbms::Statement>& dbms::Database::findStatement(const char* name)
   throw(basis::RuntimeException)
{
   logger::TraceMethod ttmm(logger::Level::Local7, COFFEE_FILE_LOCATION);

   Statement* result(NULL);

   LOG_DEBUG("Name=" << name);

   for(statement_iterator ii = statement_begin(), maxii = statement_end(); ii != maxii; ii ++) {
      if(statement(ii)->getName() == name) {
         return statement(ii);
      }
   }

   COFFEE_THROW_EXCEPTION("Statement '" << name << "' was not found");
}

void dbms::Database::notifyRecoveryFail(dbms::Connection& connection) const
   throw(basis::RuntimeException)
{
   LOG_WARN(connection.asString());

   if(m_failRecoveryHandler)
      m_failRecoveryHandler->apply(connection);
}

basis::StreamString dbms::Database::asString() const
   noexcept
{
   basis::StreamString result("dbms::Database { ");

   result << app::Service::asString();
   result << " | Name=" << m_name;

   if(m_statementTranslator)
      result << " | StatementTranslator=" << m_statementTranslator->getName();

   return result += " }";
}

std::shared_ptr<xml::Node> dbms::Database::asXML(std::shared_ptr<xml::Node>& parent) const
   noexcept
{
   std::shared_ptr<xml::Node> result = parent->createChild("dbms.Database");

   app::Service::asXML(result);

   result->createAttribute("Name", m_name);

   if(m_statementTranslator)
      result->createAttribute("Translator", m_statementTranslator->getName());

   std::shared_ptr<xml::Node> connections = result->createChild("Connections");
   for(const_connection_iterator ii = connection_begin(), maxii = connection_end(); ii != maxii; ii ++)
      connection(ii)->asXML(connections);

   std::shared_ptr<xml::Node> statements = result->createChild("Statements");
   for(const_statement_iterator ii = statement_begin(), maxii = statement_end(); ii != maxii; ii ++)
      statement(ii)->asXML(statements);

   return result;
}

