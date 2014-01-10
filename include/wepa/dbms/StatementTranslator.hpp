// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2013 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/wepa
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#ifndef _wepa_dbms_StatementTranslator_hpp
#define _wepa_dbms_StatementTranslator_hpp

#include <wepa/adt/RuntimeException.hpp>

namespace wepa {

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
 * \see wepa::dbms::Database::setStatementTranslator
 *
 * @author frr@tid.es cisco.tierra@gmail.com
 * \since NemesisRD 1.7.4
 */
class StatementTranslator {
public:
   virtual ~StatementTranslator () {;}

   /**
    * Devuelve el nombre l�gico de este traductor.
    * \return el nombre l�gico de este traductor.
    */
   const char* getName () const noexcept { return a_name; }
   
protected:
   /**
    * Constructor.
    * \param name Nombre l�gico del traductor.
    */
   explicit StatementTranslator (const char* name) : a_name (name) {;}
   
   /**
    * Se invoca autom�ticamente desde wepa::dbms::Database::createStatement si la
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
   const char* a_name;
   
   StatementTranslator (const StatementTranslator&);
   
   friend class Database;
};

}
}

#endif


