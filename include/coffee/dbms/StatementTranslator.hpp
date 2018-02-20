#ifndef _coffee_dbms_StatementTranslator_hpp
#define _coffee_dbms_StatementTranslator_hpp

#include <coffee/adt/RuntimeException.hpp>

namespace coffee {

namespace dbms {

class Database;

/**
 * Clase que ajustar de forma transparente las diferencias de tratamiento que tiene las 
 * sentencias SQL en los distintos motores de base de datos. De esta forma una aplicaci�n
 * originariamente escrita para un determinado RDBMS no tendr� que hacer ning�n cambio
 * en las sentencias SQL al cambiar a otro RDBMS.
 * 
 * Por ejemplo para indicar los par�metros de entrada en Oracle se indica con un
 * literal precedido de ':' o '&'. Con lo que la sentencia podr�a quedar como:
 * \code 
 * insert into foo (field1, field2) values (:f1, :f2)
 * \endcode
 * 
 * En PosgreSQL (tambien sorpotado en Oracle) quedar�a algo as�:
 * \code
 * insert into foo (field1, field2) values (&f1, &f2)
 * \endcode
 * 
 * Mientras que en MySQL la expresi�n deber�a ser como:
 * \code
 * insert into foo (field1, field2) values (?, ?);
 * \endcode
 * 
 * \see coffee::dbms::Database::setStatementTranslator
 *
 * @author cisco.tierra@gmail.com
 */
class StatementTranslator {
public:
   virtual ~StatementTranslator () {;}

   /**
    * Devuelve el nombre l�gico de este traductor.
    * \return el nombre l�gico de este traductor.
    */
   const std::string& getName () const noexcept { return a_name; }
   
protected:
   /**
    * Constructor.
    * \param name Nombre l�gico del traductor.
    */
   explicit StatementTranslator (const char* name) : a_name (name) {;}
   
   /**
    * Se invoca autom�ticamente desde coffee::dbms::Database::createStatement si la
    * instancia de la base de datos tiene asociada alguna instancia heredada de esta clase.
    * 
    * Éste metodo s�lo se invoca una vez para cada una de las sentencias definidas sobre
    * la base de datos, por lo que la traducci�n de sentencias SQL tiene un consumo despreciable
    * con respecto al tiempo total del proceso.
    * 
    * \param statement Sentencia SQL original.
    * \return La sentencia SQL correspondiente con la original, pero tratada para que 
    * pueda ser interpretada correctamente por el motor de base de datos sobre el que 
    * se va a ejecutar. 
    */ 
   virtual const char* apply (const char* statement) throw (adt::RuntimeException) = 0;
   
private:
   const std::string a_name;
   
   StatementTranslator (const StatementTranslator&);
   
   friend class Database;
};

}
}

#endif


