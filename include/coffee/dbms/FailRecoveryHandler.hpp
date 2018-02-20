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

#ifndef _coffee_dbms_FailRecoveryHandler_hpp
#define _coffee_dbms_FailRecoveryHandler_hpp

#include <coffee/adt/RuntimeException.hpp>
  
namespace coffee {

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
      
      \warning Este metodo se invocara automaticamente desde coffee::dbms::Database::recover.
   */
   virtual void apply (const Connection& connection) throw (adt::RuntimeException) = 0;
   
   friend class Database;
};

}
}

#endif



