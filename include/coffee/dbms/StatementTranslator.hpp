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

#ifndef _coffee_dbms_StatementTranslator_hpp
#define _coffee_dbms_StatementTranslator_hpp

#include <coffee/adt/RuntimeException.hpp>

namespace coffee {

namespace dbms {

class Database;

/**
 * Class to translate sentences between different SQL flavors.
 * 
 * It will offer the possibility to write our sentences for a very specify RDBMS-SQL
 * but they will be used without any kind of re-writing for any other RDBMS.
 *
 * For example Oracle uses ':' or '&' to indicate one SQL argument, then a valid
 * SQL sentence could look like:
 *
 * \code 
 * insert into foo (field1, field2) values (:f1, :f2)
 * \endcode
 * 
 * PosgreSQL uses '&':
 * \code
 * insert into foo (field1, field2) values (&f1, &f2)
 * \endcode
 * 
 * MySQL uses '?' without any kind of naming:
 *
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


