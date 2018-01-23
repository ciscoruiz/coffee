// COFFEE - COmpany eFFEEctive Platform
#ifndef _wepm_dbms_Database_h
#define _wepm_dbms_Database_h

#include <vector>
#include <memory>

#include <coffee/dbms/DatabaseException.hpp>
#include <coffee/dbms/ActionOnError.hpp>
#include "../app/Engine.hpp"

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
class FailRecoveryHandler;
class StatementTranslator;
class ErrorCodeInterpreter;

/**
   Clase que modela la interaccion entre la base y nuestra aplicacion.

   A diferencia de la plataforma SDP original NemesisRD.dbms permite la conexion simultanea con cualquier
   numero de base de datos.

   @author cisco.tierra@gmail.com.
*/
class Database : public app::Engine {
protected:
   typedef std::vector<std::shared_ptr <Connection> > connection_container;
   typedef connection_container::iterator connection_iterator;

   typedef std::vector<std::shared_ptr <Statement> > statement_container;
   typedef statement_container::iterator statement_iterator;

public:
   /**
      Numero maximo de conexiones que podemos crear.
   */
   static const int MaxConnection = 32;

   /**
      Formas de conexion a la base de datos.
   */
   struct Type { enum _v { Local, Remote }; };

   typedef connection_container::const_iterator const_connection_iterator; /**<Iterador para acceder a las conexiones de esta base de datos */
   typedef statement_container::const_iterator const_statement_iterator; /**<Iterador para acceder a las conexiones de esta base de datos */

   /**
      Destructor.
   */
   virtual ~Database();

   /**
      Devuelve el tipo de conexion de esta base de datos.
      \return El tipo de conexion de esta base de datos.
   */
   const Type::_v getType() const noexcept { return m_type; }

   /**
      Devuelve el nombre de la base de datos indicado en el constructor.
      \return El nombre de la base de datos indicado en el constructor.
   */
   const std::string& getName() const noexcept { return m_name; }

   void externalInitialize() throw(adt::RuntimeException);
   void externalStop() throw(adt::RuntimeException);

   void setFailRecoveryHandler(std::shared_ptr<FailRecoveryHandler> failRecoveryHandler)
      noexcept
   {
      m_failRecoveryHandler = failRecoveryHandler;
   }

   void setStatementTranslator(std::shared_ptr<StatementTranslator> statementTranslator)
      noexcept
   {
      m_statementTranslator = statementTranslator;
   }

   void setErrorCodeInterpreter(std::shared_ptr<ErrorCodeInterpreter> errorCodeInterpreter)
      noexcept
   {
      m_errorCodeInterpreter = errorCodeInterpreter;
   }

   const std::shared_ptr<ErrorCodeInterpreter>& getErrorCodeInterpreter() const noexcept { return m_errorCodeInterpreter; }

   std::shared_ptr<Connection> createConnection(const char* name, const char* user, const char* password)
      throw(adt::RuntimeException, DatabaseException);
   std::shared_ptr<Connection> createConnection(const std::string& name, const char* user, const char* password)
      throw(adt::RuntimeException, DatabaseException)
   {
      return createConnection(name.c_str(), user, password);
   }
   std::shared_ptr<Connection>& findConnection(const char* name) throw(adt::RuntimeException);
   const_connection_iterator connection_begin() const noexcept { return m_connections.begin(); }
   const_connection_iterator connection_end() const noexcept { return m_connections.end(); }
   static const std::shared_ptr<Connection>& connection(const_connection_iterator ii) noexcept { return std::ref(*ii); }

   std::shared_ptr<Statement> createStatement(const char* name, const char* expression, const ActionOnError::_v actionOnError = ActionOnError::Rollback)
      throw(adt::RuntimeException);
   std::shared_ptr<Statement> createStatement(const char* name, const std::string& expression, const ActionOnError::_v actionOnError = ActionOnError::Rollback)
      throw(adt::RuntimeException)
   {
      return createStatement(name, expression.c_str(), actionOnError);
   }
   std::shared_ptr<Statement>& findStatement(const char* name) throw(adt::RuntimeException);
   const_statement_iterator statement_begin() const noexcept { return m_statements.begin(); }
   const_statement_iterator statement_end() const noexcept { return m_statements.end(); }
   static const std::shared_ptr<Statement>& statement(const_statement_iterator ii) noexcept { return std::ref(*ii); }

   virtual adt::StreamString asString() const noexcept;

   virtual xml::Node& asXML(xml::Node& parent) const noexcept;

   Database& operator=(const Database&) = delete;
   Database(const Database&) = delete;

protected:
   /**
      Contructor.
      \param app Instancia de la aplicaci�n sobre la que corre.
      \param rdbmsName Nombre del RDMS que gestiona esta base de datos.
      \param dbmsName Nombre de la base de datos.
   */
   Database(app::Application& app, const char* rdbmsName, const char* dbmsName);

   Database(const char* rdbmsName, const char* dbmsName);

   void notifyRecoveryFail(Connection& connection) const throw(adt::RuntimeException);

   /**
      Inicializa las conexiones definidas sobre esta base de datos. Este metodo se invocaria
      automaticamente desde el nucleo de NemesisRD.
      Slo seria necesario invocarlo cuando nuestro programa no tenga asociada ninguna aplicacion
      que se encarga de inicializar los enginees.
   */
   virtual void do_initialize() throw(adt::RuntimeException);

   /**
      Elimina las conexiones definidas sobre esta base de datos. Este metodo se invocaria automaticamente
      desde el nucleo de NemesisRD.
   */
   virtual void do_stop() noexcept;

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
   const std::string m_name;
   connection_container m_connections;
   statement_container m_statements;
   const Type::_v m_type;
   mutable std::shared_ptr<FailRecoveryHandler> m_failRecoveryHandler;
   std::shared_ptr<StatementTranslator> m_statementTranslator;
   std::shared_ptr<ErrorCodeInterpreter> m_errorCodeInterpreter;

   virtual std::shared_ptr<Connection> allocateConnection(const std::string& name, const char* user, const char* password)
      throw(adt::RuntimeException) = 0;

   virtual std::shared_ptr<Statement> allocateStatement(const char* name, const std::string& expression, const ActionOnError::_v actionOnError)
      throw(adt::RuntimeException) = 0;

   virtual std::shared_ptr<binder::Input> allocateInputBind(std::shared_ptr<datatype::Abstract> data) const
      throw(adt::RuntimeException) = 0;
   virtual std::shared_ptr<binder::Output> allocateOutputBind(std::shared_ptr<datatype::Abstract> data) const
      throw(adt::RuntimeException) = 0;

   friend class Statement;
   friend class ResultCode;
   friend class Connection; // for breakConnection
};

}
}

#endif
