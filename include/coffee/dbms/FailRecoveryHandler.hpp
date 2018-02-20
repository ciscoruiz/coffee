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



