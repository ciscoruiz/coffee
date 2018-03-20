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

#ifndef _coffee_dbms_Connection_h
#define _coffee_dbms_Connection_h

#include <memory>
#include <mutex>

#include <coffee/balance/Resource.hpp>

#include <coffee/dbms/DatabaseException.hpp>
#include <coffee/dbms/ResultCode.hpp>

namespace coffee {

namespace xml {
class Node;
}

namespace dbms {

class Database;
class Statement;
class GuardConnection;
class ConnectionParameters;

/**
   Clase que modela la conexion con la base de datos.

   Para crear una nueva conexion hay que invocar al Metodo Database::createConnection de la base de datos
   contra la que deseamos establecer la conexion.

   Para obtener una conexion a una determinada base de datos habria que instanciar dicha base de datos
   e invocar al Metodo Database::createConnection. Independientemente del tipo de conexion particular que la base
   de datos retorne, debemos asignarlo a un puntero de tipo coffee::dbms::Connection.

   Antes de pasar a usar cada uno de las instancias de esta clase debe establecerse una secci�n cr�tica mediante
   GuardConnection, que deber� estar activa durante todo el proceso de uso.

   @author cisco.tierra@gmail.com.
*/
class Connection : public balance::Resource {
public:
   const Database& getDatabase() const noexcept { return m_dbmsDatabase; }

   const std::string& getUser() const noexcept { return m_user; }

   const std::string& getPassword() const noexcept { return m_password; }

   operator adt::StreamString() const noexcept { return asString(); }

   virtual adt::StreamString asString() const noexcept;

   virtual std::shared_ptr<xml::Node> asXML(std::shared_ptr<xml::Node>& parent) const noexcept;

   Connection(const Connection&) = delete;
   Connection& operator=(const Connection&) = delete;

   int getCommitPendingCounter() const noexcept { return m_commitPending; }

protected:
   const Database& m_dbmsDatabase;
   std::string m_user; /**< User name */
   std::string m_password; /**< Password */

   Connection(const Database& dbmsDatabase, const std::string& name, const ConnectionParameters& parameters);

   int setMaxCommitPending(const int maxCommitPending) noexcept {
      const int result = m_maxCommitPending;
      m_maxCommitPending = maxCommitPending;
      return result;
   }

   void clearMaxCommitPending() noexcept { m_maxCommitPending = 0;}

   void resetRollbackPending() noexcept { m_rollbackPending =  false; }

   void activateRollbackPending() noexcept { m_rollbackPending = true; }

   ResultCode execute(std::shared_ptr<Statement>& statement) throw(adt::RuntimeException, DatabaseException);

   void commit() throw(adt::RuntimeException, DatabaseException);

   void rollback() noexcept;

private:
   int m_commitPending; // Numero de sentencias a las que no se han fijado cambios.
   bool m_rollbackPending;
   int m_maxCommitPending;
   int m_lockingCounter;
   unsigned int m_accesingCounter;
   std::mutex m_mutex;

   void lock() throw(adt::RuntimeException);
   void unlock() noexcept;
   bool recover() noexcept;
   bool reachMaxCommitPending() const noexcept { return m_maxCommitPending > 0 && m_commitPending >= m_maxCommitPending; }

   virtual bool do_beginTransaction() throw(adt::RuntimeException, DatabaseException) { return false;}
   virtual void do_commit() throw(adt::RuntimeException, DatabaseException) = 0;
   virtual void do_rollback() noexcept = 0;
   virtual void open() throw(DatabaseException) = 0;
   virtual void close() noexcept = 0;

   friend class Database;
   friend class GuardConnection;
};

}
}

#endif
