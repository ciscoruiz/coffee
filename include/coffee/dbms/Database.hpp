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

#ifndef _coffee_dbms_Database_h
#define _coffee_dbms_Database_h

#include <vector>
#include <memory>
#include <typeinfo>

#include <coffee/dbms/DatabaseException.hpp>
#include <coffee/dbms/ActionOnError.hpp>
#include <coffee/app/Service.hpp>
#include <coffee/dbms/StatementParameters.hpp>

namespace coffee {

namespace dbms {

class ResultCode;
class Connection;

namespace binder{
class Input;
class Output;
}

namespace datatype {
class Abstract;
}

class Statement;
class StatementParameters;
class FailRecoveryHandler;
class StatementTranslator;
class ErrorCodeInterpreter;
class ConnectionParameters;

/**
   Class for modeling access to any kind of Database
*/
class Database : public app::Service {
protected:
   typedef std::vector<std::shared_ptr <Connection> > connection_container;
   typedef connection_container::iterator connection_iterator;

   typedef std::vector<std::shared_ptr <Statement> > statement_container;
   typedef statement_container::iterator statement_iterator;

public:
   /**
    * Max number of connections
    */
   static const int MaxConnection = 32;

   typedef connection_container::const_iterator const_connection_iterator; /**<Iterador para acceder a las conexiones de esta base de datos */
   typedef statement_container::const_iterator const_statement_iterator; /**<Iterador para acceder a las conexiones de esta base de datos */

   /**
      Destructor.
   */
   virtual ~Database();

   /**
    * \return The logical name for this instance.
    */
   const std::string& getName() const noexcept { return m_name; }

   /**
    * Set the instance to call when a database connection is lost.
    */
   void setFailRecoveryHandler(std::shared_ptr<FailRecoveryHandler> failRecoveryHandler)
      noexcept
   {
      m_failRecoveryHandler = failRecoveryHandler;
   }

   /**
    * Set the instance for do statement translator.
    */
   void setStatementTranslator(std::shared_ptr<StatementTranslator> statementTranslator)
      noexcept
   {
      m_statementTranslator = statementTranslator;
   }

   /**
    * Set the instance for error code interpreter.
    */
   void setErrorCodeInterpreter(std::shared_ptr<ErrorCodeInterpreter> errorCodeInterpreter)
      noexcept
   {
      m_errorCodeInterpreter = errorCodeInterpreter;
   }

   /**
    * \return the instance of error code interpreter associated to this database.
    */
   const std::shared_ptr<ErrorCodeInterpreter>& getErrorCodeInterpreter() const noexcept { return m_errorCodeInterpreter; }

   /**
    * Create a new connection.
    */
   std::shared_ptr<Connection> createConnection(const char* name, const ConnectionParameters& parameters)
      throw(basis::RuntimeException, DatabaseException);

   /**
    * Create a new connection.
    */
   std::shared_ptr<Connection> createConnection(const std::string& name, const ConnectionParameters& parameters)
      throw(basis::RuntimeException, DatabaseException)
   {
      return createConnection(name.c_str(), parameters);
   }

   /**
    * \return the connection that matches with the received name or throws an exception in case of there is not any connection with that name.
    */
   std::shared_ptr<Connection>& findConnection(const char* name) throw(basis::RuntimeException);

   const_connection_iterator connection_begin() const noexcept { return m_connections.begin(); }
   const_connection_iterator connection_end() const noexcept { return m_connections.end(); }
   static const std::shared_ptr<Connection>& connection(const_connection_iterator ii) noexcept { return std::ref(*ii); }

   std::shared_ptr<Statement> createStatement(const char* name, const char* expression, const StatementParameters& parameters = defaultParameters)
      throw(basis::RuntimeException);
   std::shared_ptr<Statement> createStatement(const char* name, const std::string& expression, const StatementParameters& parameters = defaultParameters)
      throw(basis::RuntimeException)
   {
      return createStatement(name, expression.c_str(), parameters);
   }

   std::shared_ptr<Statement>& findStatement(const char* name) throw(basis::RuntimeException);
   const_statement_iterator statement_begin() const noexcept { return m_statements.begin(); }
   const_statement_iterator statement_end() const noexcept { return m_statements.end(); }
   static const std::shared_ptr<Statement>& statement(const_statement_iterator ii) noexcept { return std::ref(*ii); }

   virtual basis::StreamString asString() const noexcept;

   virtual std::shared_ptr<xml::Node> asXML(std::shared_ptr<xml::Node>& parent) const noexcept;

   Database& operator=(const Database&) = delete;
   Database(const Database&) = delete;

protected:
   /**
      Contructor.
      \param app Application instance which will start database
      \param implementation RDBMS name for this database.
      \param dbmsName Logical name from this instance of the database.
   */
   Database(app::Application& app, const std::string& implementation, const char* dbmsName);

   /**
    * This method will be called when some layer detect some fault in the connection
    * received as parameter.
    */
   void notifyRecoveryFail(Connection& connection) const throw(basis::RuntimeException);

   /**
      Inicializa las conexiones definidas sobre esta base de datos. Este metodo se invocaria
      automaticamente desde el nucleo de NemesisRD.
      Slo seria necesario invocarlo cuando nuestro programa no tenga asociada ninguna aplicacion
      que se encarga de inicializar los enginees.
   */
   virtual void do_initialize() throw(basis::RuntimeException);

   /**
      Elimina las conexiones definidas sobre esta base de datos. Este metodo se invocaria automaticamente
      desde el nucleo de NemesisRD.
   */
   virtual void do_stop() throw(basis::RuntimeException);

   /**
      Devuelve un iterator al comienzo de la lista de conexiones establecidas con esta base de datos.
      \return Un iterator al comienzo de la lista de conexiones establecidas con esta base de datos.
      \since NemesisRD.dbms 1.1.1
   */
   connection_iterator connection_begin() noexcept { return m_connections.begin(); }

   /**
      Devuelve un iterator al final de la lista de conexiones establecidas con esta base de datos.
      \return Un iterator al final de la lista de conexiones establecidas con esta base de datos.
      \since NemesisRD.dbms 1.1.1
   */
   connection_iterator connection_end() noexcept { return m_connections.end(); }

   /**
      Devuelve el objeto sobre el que esta posicionado el iterator recibido como parametro.
      \param ii Iterator que deberia estar comprendido entre #connection_begin y #connection_end.
      \return El objeto sobre el que esta posicionado el iterator recibido como parametro.
      \since NemesisRD.dbms 1.1.1
   */
   static std::shared_ptr<Connection>& connection(connection_iterator ii) noexcept { return std::ref(*ii); }

   /**
      Devuelve un iterator al comienzo de la lista de sentencias SQL creadas en esta base de datos.
      \return Un iterator al comienzo de la lista de sentencias SQL creadas en esta base de datos.
      \since NemesisRD.dbms 1.2.2
   */
   statement_iterator statement_begin() noexcept { return m_statements.begin(); }

   /**
      Devuelve un iterator al final de la lista de sentencias SQL creadas en esta base de datos.
      \return Un iterator al final de la lista de sentencias SQL creadas en esta base de datos.
      \since NemesisRD.dbms 1.2.2
   */
   statement_iterator statement_end() noexcept { return m_statements.end(); }

   /**
      Devuelve el objeto sobre el que esta posicionado el iterator recibido como parametro.
      \param ii Iterator que deberia estar comprendido entre #statement_begin y #statement_end.
      \return El objeto sobre el que esta posicionado el iterator recibido como parametro.
      \since NemesisRD.dbms 1.2.2
   */
   static std::shared_ptr<Statement>& statement(statement_iterator ii) noexcept { return std::ref(*ii); }

private:
   static StatementParameters defaultParameters;

   const std::string m_name;
   connection_container m_connections;
   statement_container m_statements;
   mutable std::shared_ptr<FailRecoveryHandler> m_failRecoveryHandler;
   std::shared_ptr<StatementTranslator> m_statementTranslator;
   std::shared_ptr<ErrorCodeInterpreter> m_errorCodeInterpreter;

   virtual std::shared_ptr<Connection> allocateConnection(const std::string& name, const ConnectionParameters& parameters)
      throw(basis::RuntimeException, std::bad_cast) = 0;

   virtual std::shared_ptr<Statement> allocateStatement(const char* name, const std::string& expression, const StatementParameters& parameters)
      throw(basis::RuntimeException, std::bad_cast) = 0;

   virtual std::shared_ptr<binder::Input> allocateInputBind(std::shared_ptr<datatype::Abstract> data) const
      throw(basis::RuntimeException) = 0;
   virtual std::shared_ptr<binder::Output> allocateOutputBind(std::shared_ptr<datatype::Abstract> data) const
      throw(basis::RuntimeException) = 0;

   friend class Statement;
   friend class ResultCode;
   friend class Connection; // for breakConnection
};

}
}

#endif
