// WEPA - Write Excellent Professional Applications
#ifndef _wepm_dbms_Database_h
#define _wepm_dbms_Database_h

#include <boost/ptr_container/ptr_vector.hpp>

#include <wepa/app/EngineIf.hpp>

#include <wepa/dbms/DatabaseException.hpp>

namespace wepa {

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

/**
   Clase que modela la interaccion entre la base y nuestra aplicacion.

   A diferencia de la plataforma SDP original NemesisRD.dbms permite la conexion simultanea con cualquier
   numero de base de datos.

   @author cisco.tierra@gmail.com.
*/
class Database : public app::EngineIf {
protected:
   typedef boost::ptr_vector <Connection> connection_container;
   typedef connection_container::iterator connection_iterator;

   typedef boost::ptr_vector <Statement> statement_container;
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
   virtual ~Database ();

   /**
      Devuelve el tipo de conexion de esta base de datos.
      \return El tipo de conexion de esta base de datos.
   */
   const Type::_v getType () const throw () { return m_type; }

   /**
      Devuelve el nombre de la base de datos indicado en el constructor.
      \return El nombre de la base de datos indicado en el constructor.
   */
   const std::string& getName () const throw () { return m_name; }

   /**
      Establece el manejador encargado de actuar cuando la recuperacion de la conexion falla.
      El manejador por defecto no realiza ninguna activad.
      \param failRecoveryHandler Manejador que seria invocado en caso de que no sea posible recuperar
      una determina conexion.
   */
   void setFailRecoveryHandler (FailRecoveryHandler* failRecoveryHandler) throw () { m_failRecoveryHandler = failRecoveryHandler; }

   /**
    * Establece el traductor de sentencias SQL usado ajustar las sentencias SQL al
    * motor de base de datos usados en la aplicaci�n.
    */
   void setStatementTranslator (StatementTranslator* statementTranslator) throw () { m_statementTranslator = statementTranslator; }

   /**
      Crea y registra una nueva conexion con esta base de datos.
      La clase usada para conectar con esta base de datos dependeria de la implementacion particular, que
      seria definida por el metodo #allocateConnection.

      \param name Nombre logico de la conexion a crear.
      @param user Nombre del usuario con el que realizamos la conexion.
      @param password Codigo de acceso del usuario.

      @return La instancia de la nueva conexion a la base de datos.
   */
   Connection* createConnection (const char* name, const char* user, const char* password)
      throw (adt::RuntimeException, DatabaseException);

   /**
      Devuelve la conexion asociada al nombre logico recibido como parametro.
      \param name Nombre logico de la conexion que queremos obtener.
      \return La conexion asociada al nombre logico recibido como parametro.
      \warning Si la conexion logica no existe no puede ser usada se lanzara una excepcion.
      \since NemesisRD.dbms 1.1.1
   */
   Connection& findConnection (const char* name) throw (adt::RuntimeException);

   /**
      Devuelve un iterator al comienzo de la lista de conexiones establecidas con esta base de datos.
      \return Un iterator al comienzo de la lista de conexiones establecidas con esta base de datos.
   */
   const_connection_iterator connection_begin () const throw () { return m_connections.begin (); }

   /**
      Devuelve un iterator al final de la lista de conexiones establecidas con esta base de datos.
      \return Un iterator al final de la lista de conexiones establecidas con esta base de datos.
   */
   const_connection_iterator connection_end () const throw () { return m_connections.end (); }

   /**
      Devuelve el objeto sobre el que esta posicionado el iterator recibido como parametro.
      \param ii Iterator que deberia estar comprendido entre #connection_begin y #connection_end.
      \return El objeto sobre el que esta posicionado el iterator recibido como parametro.
      \since NemesisRD.dbms 1.0.2
   */
   static const Connection& connection (const_connection_iterator ii) throw () { return std::ref (*ii); }

   /**
      Crea y registra una nueva sentencia SQL asociada a esta base de datos.
      La clase usada para interpretar la sentencia SQL dependera de la implementacion particular definida
      mediante el metodo #allocateStatement.

      \param name Nombre logico de esta sentencia.
      \param expression Expresion asociada a la sentencia.
      \param isCritical Si vale \em true indica que si la ejecucion de esta sentencia falla al desbloquear
      la conexion con la que ejecutamos esta sentencia se invocara a Connection::rollback, en otro caso
      aunque falle se invocara a Connection::commit. Solo aplicara en sentencias que no sean de seleccion.

      \return Una nueva instancia de una conexion a base de datos. No puede ser NULL.
   */
   Statement* createStatement (const char* name, const char* expression, const bool isCritical = true)
      throw (adt::RuntimeException);

   /**
      Crea y registra una nueva sentencia SQL asociada a esta base de datos.
      La clase usada para interpretar la sentencia SQL dependera de la implementacion particular definida
      mediante el metodo #allocateStatement.

      \param name Nombre logico de esta sentencia.
      \param expression Expresion asociada a la sentencia.
      \param isCritical Si vale \em true indica que si la ejecucion de esta sentencia falla al desbloquear
      la conexion con la que ejecutamos esta sentencia se invocara a Connection::rollback, en otro caso
      aunque falle se invocara a Connection::commit. Solo aplicara en sentencias que no sean de seleccion.

      \return Una nueva instancia de una conexion a base de datos. No puede ser NULL.
   */
   Statement* createStatement (const char* name, const std::string& expression, const bool isCritical = true)
      throw (adt::RuntimeException)
   {
      return createStatement (name, expression.c_str (), isCritical);
   }

   /**
      Devuelve la instancia de la sentencia SQL asociada al nombre recibido como parametro.

      @return La instancia de la sentencia SQL  asociada al nombre recibido.
      Puede ser NULL si el nombre no fue registrado previamente con #createStatement.
   */
   Statement& findStatement (const char* name) throw (adt::RuntimeException);

   /**
      Devuelve un iterator al comienzo de la lista de sentencias SQL creadas en esta base de datos.
      \return Un iterator al comienzo de la lista de sentencias SQL creadas en esta base de datos.
      \since NemesisRD.dbms 1.2.2
   */
   const_statement_iterator statement_begin () const throw () { return m_statements.begin (); }

   /**
      Devuelve un iterator al final de la lista de sentencias SQL creadas en esta base de datos.
      \return Un iterator al final de la lista de sentencias SQL creadas en esta base de datos.
      \since NemesisRD.dbms 1.2.2
   */
   const_statement_iterator statement_end () const throw () { return m_statements.end (); }

   /**
      Devuelve el objeto sobre el que esta posicionado el iterator recibido como parametro.
      \param ii Iterator que deberia estar comprendido entre #statement_begin y #statement_end.
      \return El objeto sobre el que esta posicionado el iterator recibido como parametro.
      \since NemesisRD.dbms 1.2.2
   */
   static const Statement& statement (const_statement_iterator ii) throw () { return std::ref (*ii); }

   /**
      Devuelve una cadena con la informacion mas relevante de esta instancia.
      \return Una cadena con la informacion mas relevante de esta instancia.
   */
   virtual adt::StreamString  asString () const throw ();

   /**
      Devuelve un documento XML con la informacion mas relevante de esta instancia.
      \param parent Nodo XML del que colgar la informacion referente a esta instancia.
      \return Un documento XML con la informacion mas relevante de esta instancia.
   */
   virtual xml::Node& asXML (xml::Node& parent) const throw ();

protected:
   /**
      Contructor.
      \param app Instancia de la aplicaci�n sobre la que corre.
      \param rdbmsName Nombre del RDMS que gestiona esta base de datos.
      \param dbmsName Nombre de la base de datos.
   */
   Database (app::Application& app, const char* rdbmsName, const char* dbmsName);

   /**
      M�todo invocado desde el n�cleo cuando se detecta que una conexi�n ha dejado de funcionar. Por ahora
      s�lo se invocar� desde dbms::Connection::execute.
      \warning Este metodo se invoca automaticamente desde el nucleo de NemesisRD.dbms y nunca deberia
      ser invocado por el programador.
      \param connection Instancia de la conexion en la que hemos detectado el fallo.
   */
   void breakConnection (Connection& connection) throw (adt::RuntimeException);

   /**
      Inicializa las conexiones definidas sobre esta base de datos. Este metodo se invocaria
      automaticamente desde el nucleo de NemesisRD.
      Slo seria necesario invocarlo cuando nuestro programa no tenga asociada ninguna aplicacion
      que se encarga de inicializar los enginees.
   */
   virtual void do_initialize () throw (adt::RuntimeException);

   /**
      Elimina las conexiones definidas sobre esta base de datos. Este metodo se invocaria automaticamente
      desde el nucleo de NemesisRD.
   */
   virtual void do_stop () throw ();

   /**
      Devuelve un iterator al comienzo de la lista de conexiones establecidas con esta base de datos.
      \return Un iterator al comienzo de la lista de conexiones establecidas con esta base de datos.
      \since NemesisRD.dbms 1.1.1
   */
   connection_iterator connection_begin () throw () { return m_connections.begin (); }

   /**
      Devuelve un iterator al final de la lista de conexiones establecidas con esta base de datos.
      \return Un iterator al final de la lista de conexiones establecidas con esta base de datos.
      \since NemesisRD.dbms 1.1.1
   */
   connection_iterator connection_end () throw () { return m_connections.end (); }

   /**
      Devuelve el objeto sobre el que esta posicionado el iterator recibido como parametro.
      \param ii Iterator que deberia estar comprendido entre #connection_begin y #connection_end.
      \return El objeto sobre el que esta posicionado el iterator recibido como parametro.
      \since NemesisRD.dbms 1.1.1
   */
   static Connection& connection (connection_iterator ii) throw () { return std::ref (*ii); }

   /**
      Devuelve un iterator al comienzo de la lista de sentencias SQL creadas en esta base de datos.
      \return Un iterator al comienzo de la lista de sentencias SQL creadas en esta base de datos.
      \since NemesisRD.dbms 1.2.2
   */
   statement_iterator statement_begin () throw () { return m_statements.begin (); }

   /**
      Devuelve un iterator al final de la lista de sentencias SQL creadas en esta base de datos.
      \return Un iterator al final de la lista de sentencias SQL creadas en esta base de datos.
      \since NemesisRD.dbms 1.2.2
   */
   statement_iterator statement_end () throw () { return m_statements.end (); }

   /**
      Devuelve el objeto sobre el que esta posicionado el iterator recibido como parametro.
      \param ii Iterator que deberia estar comprendido entre #statement_begin y #statement_end.
      \return El objeto sobre el que esta posicionado el iterator recibido como parametro.
      \since NemesisRD.dbms 1.2.2
   */
   static Statement& statement (statement_iterator ii) throw () { return std::ref (*ii); }

private:
   const std::string m_name;
   connection_container m_connections;
   statement_container m_statements;
   const Type::_v m_type;
   FailRecoveryHandler* m_failRecoveryHandler;
   StatementTranslator* m_statementTranslator;

   Database (const Database&);

   static Connection* connection_ptr (connection_iterator ii) throw () { Connection& result = std::ref (*ii); return &result; }
   static Statement* statement_ptr (statement_iterator ii) throw () { Statement& result = std::ref (*ii); return &result; }

   virtual Connection* allocateConnection (const std::string& name, const char* user, const char* password)
      throw (adt::RuntimeException) = 0;

   virtual Statement* allocateStatement (const char* name, const std::string& expression, const bool isCritical)
      throw (adt::RuntimeException) = 0;

   virtual binder::Input* allocateInputBind (datatype::Abstract& data) throw (adt::RuntimeException) = 0;
   virtual binder::Output* allocateOutputBind (datatype::Abstract& data) throw (adt::RuntimeException) = 0;

   virtual bool notFound (const int errorCode) const throw () = 0;
   virtual bool successful (const int errorCode) const throw () = 0;
   virtual bool locked (const int errorCode) const throw () = 0;
   virtual bool lostConnection (const int errorCode) const throw () = 0;

   friend class Statement;
   friend class ResultCode;
   friend class Connection; // for breakConnection
};

}
}

#endif
