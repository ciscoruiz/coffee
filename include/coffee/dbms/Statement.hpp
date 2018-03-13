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

#ifndef _coffee_dbms_Statement_h
#define _coffee_dbms_Statement_h

#include <vector>
#include <mutex>
#include <memory>
#include <chrono>

#include <coffee/adt/RuntimeException.hpp>
#include <coffee/adt/Average.hpp>
#include <coffee/adt/Microsecond.hpp>

#include <coffee/dbms/DatabaseException.hpp>
#include <coffee/dbms/ResultCode.hpp>
#include <coffee/dbms/ActionOnError.hpp>

namespace coffee {

namespace xml {
class Node;
}

namespace dbms {

class Database;
class Connection;
class GuardStatement;

namespace binder {
class Input;
class Output;
}

namespace datatype {
class Abstract;
}

/**
   Clase que facilita la ejecucion de sentencias SQL.

   Para obtener la instancia de un comando para una determinada base de datos habra que instanciar
   dicha base de datos e invocar al metodo Database::createStatement. Independientemente del tipo de comando
   particular que la base de datos retorne, debemos asignarlo a un puntero de tipo coffee::dbms::Statement.

   @author cisco.tierra@gmail.com.
*/
class Statement {
public:
   typedef std::vector<std::shared_ptr<binder::Input> > Inputs;
   typedef std::vector<std::shared_ptr<binder::Output> > Outputs;

   typedef Inputs::iterator input_iterator;
   typedef Outputs::iterator output_iterator;

   /**
      Destructor.
   */
   virtual ~Statement();

   /**
      Devuelve el nombre logico de esta sentencia.
      \return El nombre logico de esta sentencia.
   */
   const std::string& getName() const noexcept { return m_name; }

   /**
      Devuelve la expresion SQL recibida en el constructor.
      \return La expresion SQL recibida en el constructor.
   */
   const std::string& getExpression() const noexcept { return m_expression; }

   /**
      Devuelve el indicador de criticidad, si vale \em true indica que si la ejecucion de esta sentencia
      falla al desbloquear la conexion con la que ejecutamos esta sentencia se invocara a
      Connection::rollback, en otro caso  aunque falle se invocara a Connection::commit.
      Solo aplicara en sentencias que no sean de seleccion.
      \return El indicador de criticidad de esta sentencia.
   */
   ActionOnError::_v actionOnError() const noexcept { return m_actionOnError; }

   /**
      Devuelve la instancia de la base de datos asociada a esta sentencia.
      \return La instancia de la base de datos asociada a la sentencia.
   */
   const Database& getDatabase() const noexcept { return m_database; }

   /**
      Devuelve \em true si la sentencia requiere la invocacion a \em commit o \em rollback
      tras su ejecucion. Puede devolver \em true por tratarse de una sentencia que no tiene variables
      de salida(insert, update o delete) o bien porque se haya activado el indicador correspondiente
      mediante la llamada GuardStatement::setRequiresCommit
      \since NemesisRD.dbms 1.5.2.
   */
   bool requiresCommit() const noexcept { return (m_requiresCommit == true) || (m_outputBinds.empty() == true); }

   /**
      Establece el parametro de entrada de la sentencia SQL.Cada una de las variables de entrada indicadas
      en esta sentencia SQL deberia tener un parametro de entrada asociado. La correspondencia entre esta
      variable y la sentencia SQL vendra dada por el orden de aparacion en la sentencia SQL y por el orden
      de definicion del parametro.

      Por ejemplo la sentencia:

      \code
      update tabla1 set xx = :unavariable where yy = :otravariable;
      \endcode

      Cada una de las variables \em unavariable y \em otravariable debera tener asociado una variable de entrada.
      Primero debemos indicaremos la correspondiente a \em unavariable y luego la correspondiente a \em otravariable.

      \param data Variable que deseamos asociar como variable de entrada. La correspondencia entre esta
      y la sentencia SQL vendra dada por el orden de declaracion.
   */
   void createBinderInput(std::shared_ptr<datatype::Abstract> data) throw(adt::RuntimeException);

   /**
      Establece el parametro de salida de la sentencia SQL.Cada una de las variables de salida indicadas
      en esta sentencia SQL deberia tener un parametro de salida asociado. La correspondencia entre esta
      variable y la sentencia SQL vendra dada por el orden de aparacion en la sentencia SQL y por el orden
      de definicion del parametro.

      Por ejemplo la sentencia:

      \code
      select xx, yy from tabla1 where zz = :unavariable;
      \endcode

      Cada una de las variables \em xx e \em yy debera tener asociado una variable de salida.
       Ademas la variable \em unavaraible debera tener asociada una variable de entrada.

      \param data Variable que deseamos asociar como variable de salida. La correspondencia entre esta
      y la sentencia SQL vendra dada por el orden de declaracion.

      \return La instancia del dbms::OutputBind asociado al dato. Desde NemesisRD.dbms 1.4.2 nos permite
      grabar el contenido de los tipos de datos BLOB.

      \warning Solo las sentencias SQL del tipo \em select usan las variables de salida.
   */
   void createBinderOutput(std::shared_ptr<datatype::Abstract> data) throw(adt::RuntimeException);

   /**
      Devuelve un documento XML con la informacion referente a esta instancia.
      \param parent Nodo XML del que debe colgar la informacion.
      @return un documento XML con la informacion referente a esta instancia.
   */
   virtual std::shared_ptr<xml::Node> asXML(std::shared_ptr<xml::Node>& parent) const noexcept;

   operator adt::StreamString() const noexcept { return asString(); }

   /**
      Devuelve una cadena con la informacion referente a esta instancia.
      @return Una cadena con la informacion referente a esta instancia.
   */
   virtual adt::StreamString asString() const noexcept;

   Statement(const Statement&) = delete;
   Statement& operator=(const Statement&) = delete;

protected:
   /**
      Contructor.

      \param database Base de datos sobre la que se define la sentencia.
      \param name Nombre logico de la sentencia.
      \param expression Sentencia SQL asociada a esta clase.
      la conexion con la que ejecutamos esta sentencia se invocara a Connection::rollback, en otro caso
      aunque falle se invocara a Connection::commit. Solo aplicara en sentencias que no sean de seleccion.
      \param actionOnError Action to do in case of statement execution fails.
   */
   Statement(const Database& database, const char* name, const char* expression, const ActionOnError::_v actionOnError) :
      m_database(database),
      m_name(name),
      m_expression(expression),
      m_actionOnError(actionOnError),
      m_elapsedTime("Timing", "us"),
      m_requiresCommit(false),
      m_isPrepared(false)
   {
   }

   /**
      Contructor.

      \param database Base de datos sobre la que se define la sentencia.
      \param name Nombre logico de la sentencia.
      \param expression Sentencia SQL asociada a esta clase.
      \param actionOnError Action to do in case of statement execution fails.
   */
   Statement(const Database& database, const char* name, const std::string& expression, const ActionOnError::_v actionOnError) :
      m_database(database),
      m_name(name),
      m_expression(expression),
      m_actionOnError(actionOnError),
      m_elapsedTime("ElapsedTime", "us"),
      m_requiresCommit(false),
      m_isPrepared(false)
   {
   }

   int input_size() const noexcept { return m_inputBinds.size(); }
   int output_size() const noexcept { return m_outputBinds.size(); }
   virtual bool isPrepared(Connection&) const noexcept { return m_isPrepared; }

   std::shared_ptr<datatype::Abstract>& getInputData(const GuardStatement&, const int pos) throw(adt::RuntimeException);
   std::shared_ptr<datatype::Abstract>& getOutputData(const GuardStatement&, const int pos) throw(adt::RuntimeException);

   const std::shared_ptr<datatype::Abstract>& getInputData(const int pos) const throw(adt::RuntimeException);
   std::shared_ptr<datatype::Abstract>& getOutputData(const int pos) throw(adt::RuntimeException);

private:
   const Database& m_database;
   const std::string m_name;
   std::string m_expression;
   Inputs m_inputBinds;  /**< Lista de variables de entrada */
   Outputs m_outputBinds; /**< Lista de variables de salida */
   const ActionOnError::_v m_actionOnError;
   adt::Average <long> m_elapsedTime;
   bool m_requiresCommit;
   std::mutex m_mutex;
   bool m_isPrepared;

   void registerElapsedTime(const std::chrono::microseconds& elapsedTime) noexcept;

   void prepare(Connection& connection) throw(adt::RuntimeException, DatabaseException);
   virtual void do_prepare(Connection& connection) throw(adt::RuntimeException, DatabaseException) = 0;

   ResultCode execute(Connection& connection) throw(adt::RuntimeException, DatabaseException);
   virtual ResultCode do_execute(Connection& connection) throw(adt::RuntimeException, DatabaseException) = 0;

   bool setRequiresCommit(const bool value) noexcept { m_requiresCommit = value; return requiresCommit(); }

   bool fetch() throw(adt::RuntimeException, DatabaseException);
   virtual bool do_fetch() throw(adt::RuntimeException, DatabaseException) = 0;

   void lock() noexcept { m_mutex.lock(); }
   void unlock() noexcept { m_mutex.unlock(); }

   friend class Connection;
   friend class std::shared_ptr<Database>;
   friend class GuardStatement;
   friend class Database;
};

}
}

#endif
