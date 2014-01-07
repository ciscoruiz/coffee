// WEPA - Write Excellent Professional Applications
#ifndef _wepm_dbms_Connection_h
#define _wepm_dbms_Connection_

#include <mutex>

#include <wepa/balance/Resource.hpp>

#include <wepa/dbms/DatabaseException.hpp>
#include <wepa/dbms/ResultCode.hpp>

namespace wepa {

namespace xml {
class Node;
}

namespace dbms {

class Database;
class Statement;
class GuardConnection;

/**
   Clase que modela la conexion con la base de datos.

   Para crear una nueva conexion hay que invocar al Metodo Database::createConnection de la base de datos
   contra la que deseamos establecer la conexion.

   Para obtener una conexion a una determinada base de datos habria que instanciar dicha base de datos
   e invocar al Metodo Database::createConnection. Independientemente del tipo de conexion particular que la base
   de datos retorne, debemos asignarlo a un puntero de tipo wepa::dbms::Connection.

   Antes de pasar a usar cada uno de las instancias de esta clase debe establecerse una secci�n cr�tica mediante
   GuardConnection, que deber� estar activa durante todo el proceso de uso.

   @author frr@tid.es cisco.tierra@gmail.com.
*/
class Connection : public balance::Resource {
public:
   /**
      Devuelve la instancia de la base de datos asociada a esta conexion.
   */
   Database& getDatabase () const throw () { return m_dbmsDatabase; }

   /**
    * Devuelve el usuario con el que fu� realizada esta conexi�n.
    * \return el usuario con el que fu� realizada esta conexi�n.
    * \since NemesisRD.dbms 1.9.1
    */
   const std::string& getUser () const throw () { return m_user; }

   /**
    * Devuelve el password del usuario con el que fu� realizada esta conexi�n.
    * \return el password del usuario con el que fu� realizada esta conexi�n.
    * \since NemesisRD.dbms 1.9.1
    */
   const std::string& getPassword () const throw () { return m_password; }

   /**
      Establece el periodo de grabacion de esta conexion. Damos la posibilidad de que la conexion confirme
      cambios realizados hasta el momento sin que termine la seccion critica que debemos establecer antes
      de usar la conexion.
      \param maxCommitPending Numero de transacciones que pueden estar pedientes de confirmacion antes
      de invocar a #commit. Un valor 0, desactiva el periodo.
      \return El periodo de grabacion que habia antes de invocar a este metodo.
      \warning La invocacion a este metodo debera hacerse con una seccion critica activada sobre la
      esta conexion.
      \since NemesisRD.dbms 1.1.12
   */
   int setMaxCommitPending (const int maxCommitPending) throw () {
      const int result = m_maxCommitPending;
      m_maxCommitPending = maxCommitPending;
      return result;
   }

   /**
      Devuelve una cadena con la informacion referente a esta instancia.
      @return Una cadena con la informacion referente a esta instancia.
   */
   virtual adt::StreamString asString () const throw ();

   /**
      Devuelve un documento XML con la informacion referente a esta instancia.
      \param parent Nodo XML del que debe colgar la informacion.
      @return un documento XML con la informacion referente a esta instancia.
   */
   virtual xml::Node& asXML (xml::Node& parent) const throw ();

protected:
   /**
     Instancia de la base de datos asociada a esta conexion.
     Coincidiria con la indicada en el constructor.
   */
   Database& m_dbmsDatabase;
   std::string m_user; /**< Nombre del usuario */
   std::string m_password; /**< Clave de acceso del usuario. */

   /**
      Contructor.

      @param dbmsDatabase Instancia de la base de datos asociada a esta conexion.
      @param name Nombre logico de la conexion.
      @param user Nombre del usuario con el que realizamos la conexion.
      @param password Codigo de acceso del usuario.
   */
   Connection (Database& dbmsDatabase, const std::string& name, const char* user, const char* password) :
      balance::Resource (name),
      m_dbmsDatabase (dbmsDatabase),
      m_user (user),
      m_password (password),
      m_lockingCounter (0),
      m_commitPending (0),
      m_rollbackPending (false),
      m_maxCommitPending (0),
      m_accesingCounter (0)
   {}

   /**
      Desactiva el indicador de que la conexion requiere una invocacion a #rollback.
      \warning La invocacion a este metodo debera hacerse con una seccion critica activada sobre la
      esta conexion.
      \since NemesisRD.dbms 1.1.12
   */
   void resetRollbackPending () throw () { m_rollbackPending =  false; }

   /**
      Activa de forma externa el indicador de que la conexion requiere una invocacion a #rollback.
      \warning La invocacion a este metodo debera hacerse con una seccion critica activada sobre la
      esta conexion.
      \since NemesisRD.dbms 1.2.11
   */
   void activateRollbackPending () throw () { m_rollbackPending = true; }

   /**
      Ejecuta la sentencia recibida como parametro. Si la sentencia no tiene variables de salida consideraria
      que es un comando \em update, \em insert o \em delete, lo cual, implica la invocacion automatica a los
      #commit o #rollback cuando termine la seccion critica de esta conexion.

      \param statement Sentencia que vamos a ejecuta

      \return La estructura con el resultado de la ejecucion de la sentencia.

      \warning La invocacion a este metodo debera hacerse con una seccion critica activada sobre la
      esta conexion, por ejemplo:
      \code
            Guard guard (connection);
            connection.execute (someStatement);
      \endcode
   */
   ResultCode execute (Statement* statement) throw (adt::RuntimeException, DatabaseException);

   /**
      Ejecuta la sentencia recibida como parametro. Si la sentencia no tiene variables de salida consideraria
      que es un comando \em update, \em insert o \em delete, lo cual, implica la invocacion automatica a los
      #commit o #rollback cuando termine la seccion critica de esta conexion.

      \param statement Sentencia que vamos a ejecuta

      \return La estructura con el resultado de la ejecucion de la sentencia.

      \warning La invocacion a este metodo debera hacerse con una seccion critica activada sobre la
      esta conexion, por ejemplo:
      \code
            Guard guard (connection);
            connection.execute (someStatement);
      \endcode
   */
   ResultCode execute (Statement& statement) throw (adt::RuntimeException, DatabaseException) { return execute (&statement); }

   /**
      Metodo que fija los cambios realizados en la ejecucion de los comandos SQL.
   */
   void commit () throw (adt::RuntimeException, DatabaseException);

   /**
      Metodo que debemos re-escribir para descartar los cambios realizados sobre las tablas mediante
      esta conexion.
   */
   void rollback () throw ();

   /**
      Metodo que debemos re-escribir para hacer efectiva esta conexion.
   */
   virtual void open () throw (DatabaseException) = 0;

   /**
      Metodo que debemos re-escribir para cerrar la conexion.
   */
   virtual void close () throw () = 0;

private:
   int m_commitPending; // Numero de sentencias a las que no se han fijado cambios.
   bool m_rollbackPending;
   int m_maxCommitPending;
   int m_lockingCounter;
   unsigned int m_accesingCounter;  // N� de veces que se ha usado esta conexi�n
   std::mutex m_mutex;

   Connection (const Connection&);

   void initialize () throw (adt::RuntimeException);
   void lock () throw (adt::RuntimeException);
   void unlock () throw ();

   virtual bool do_beginTransaction () throw (adt::RuntimeException, DatabaseException) { return false;}
   virtual void do_commit () throw (adt::RuntimeException, DatabaseException) = 0;
   virtual void do_rollback () throw () = 0;

   friend class Database;

   friend class GuardConnection;
      // lock, unlock. execute, commit, rollback and so on
};

}
}

#endif
