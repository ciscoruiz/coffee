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

   @author cisco.tierra@gmail.com.
*/
class Connection : public balance::Resource {
public:
   Database& getDatabase () const noexcept { return m_dbmsDatabase; }

   const std::string& getUser () const noexcept { return m_user; }

   const std::string& getPassword () const noexcept { return m_password; }

   operator adt::StreamString () const noexcept { return asString (); }

   virtual adt::StreamString asString () const noexcept;

   virtual xml::Node& asXML (xml::Node& parent) const noexcept;

   Connection (const Connection&) = delete;
   Connection& operator= (const Connection&) = delete;

protected:
   Database& m_dbmsDatabase;
   std::string m_user; /**< Nombre del usuario */
   std::string m_password; /**< Clave de acceso del usuario. */

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

   int setMaxCommitPending (const int maxCommitPending) noexcept {
      const int result = m_maxCommitPending;
      m_maxCommitPending = maxCommitPending;
      return result;
   }

   void clearMaxCommitPending () noexcept { m_maxCommitPending = 0;}

   void resetRollbackPending () noexcept { m_rollbackPending =  false; }

   void activateRollbackPending () noexcept { m_rollbackPending = true; }

   ResultCode execute (Statement& statement) throw (adt::RuntimeException, DatabaseException);

   void commit () throw (adt::RuntimeException, DatabaseException);

   void rollback () noexcept;

private:
   int m_commitPending; // Numero de sentencias a las que no se han fijado cambios.
   bool m_rollbackPending;
   int m_maxCommitPending;
   int m_lockingCounter;
   unsigned int m_accesingCounter;
   std::mutex m_mutex;

   void lock () throw (adt::RuntimeException);
   void unlock () noexcept;
   bool recover () noexcept;

   virtual bool do_beginTransaction () throw (adt::RuntimeException, DatabaseException) { return false;}
   virtual void do_commit () throw (adt::RuntimeException, DatabaseException) = 0;
   virtual void do_rollback () noexcept = 0;
   virtual void open () throw (DatabaseException) = 0;
   virtual void close () noexcept = 0;

   friend class Database;
   friend class GuardConnection;
};

}
}

#endif
