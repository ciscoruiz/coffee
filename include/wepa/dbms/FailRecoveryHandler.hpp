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
#ifndef _wepa_dbms_FailRecoveryHandler_hpp
#define _wepa_dbms_FailRecoveryHandler_hpp

#include <wepa/adt/RuntimeException.hpp>
  
namespace wepa {

namespace dbms {

class Database;
class Connection;

/**
   Interfaz que deben cumplir los manejadores que reciben la notificacion de que no ha sido posible
   restaurar una determina conexion con la base de datos.

   @author cisco.tierra@gmail.com.
*/
class FailRecoveryHandler {
public:
   virtual ~FailRecoveryHandler () {;}

private:
   /**
      Este metodo debe ser reimplementado para describir las operaciones que vamos a realizar en caso
      de no poder recuperar la conexion recibida como parametro.
      \param connection Instancia de la conexion en la que hemos detectado el fallo.
      \param tryCounter Numero de intentos de recuperacion de la conexion.
      
      \warning Este metodo se invocara automaticamente desde wepa::dbms::Database::recover.
   */
   virtual void apply (const Connection& connection) throw (adt::RuntimeException) = 0;
   
   friend class Database;
};

}
}

#endif



