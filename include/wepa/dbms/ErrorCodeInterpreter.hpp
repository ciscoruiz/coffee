// WEPA - Write Excellent Professional Applications
#ifndef _wepm_dbms_ErrorCodeInterpreter_h
#define _wepm_dbms_ErrorCodeInterpreter_h

#include <string>
#include <memory>

#include <string.h>

#include <stdlib.h>

#include <wepa/adt/RuntimeException.hpp>

namespace wepa {

namespace dbms {

class Database;

/**
   Clase para acceder a la informacion devuelta por el gestor de base de datos
   referente al ultimo comando realizado.

   @author cisco.tierra@gmail.com
 */
class ErrorCodeInterpreter {
public:
   /**
      Devuelve \em true si las condiciones de busqueda de la ultimo operacion
      no han sido satisfechas por ningun registro o \em false en otro caso.      
      @return \em true si las condiciones de busqueda de la ultimo operacion
      no han sido satisfechas por ningun registro o \em false en otro caso.
   */   
   virtual bool notFound(const int errorCode) const throw(adt::RuntimeException) = 0;
   
   /**
      Devuelve \em true si la ultima operacion solicitada fue realizada correctamente
      o \em false en otro caso.      
      @return \em true si la ultima operacion solicitada fue realizada correctamente
      o \em false en otro caso.      
   */
   virtual bool successful(const int errorCode) const throw(adt::RuntimeException) = 0;

   /**
      Devuelve \em true Si el registro obtenenido en una sentencia de seleccion con indicador
      de modo exclusivo ha sido bloqueada previamente por otro proceso y/o contexto de base de 
      datos o \em false en otro caso.      
      @return \em true Si el registro obtenenido en una sentencia de seleccion con indicador
      de modo exclusivo ha sido bloqueada previamente por otro proceso y/o contexto de base de 
      datos o \em false en otro caso.      
   */
   virtual bool locked(const int errorCode) const throw(adt::RuntimeException) = 0;

   /**
      Devuelve \em true si se perdio la conexion la base de datos o \em false en otro caso.
      @return \em true si se perdio la conexion la base de datos o \em false en otro caso.
   */
   virtual bool lostConnection(const int errorCode) const throw(adt::RuntimeException) = 0;
};

}
}

#endif

