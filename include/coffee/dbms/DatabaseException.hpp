#ifndef _wepm_dbms_DatabaseException_hpp
#define _wepm_dbms_DatabaseException_hpp

#include <coffee/adt/Exception.hpp>

#include <coffee/dbms/ResultCode.hpp>

#include <coffee/adt/StreamString.hpp>

namespace coffee {

namespace dbms {

/**
   Exception ocurrida al acceder a algun servicio de la base de datos.
*/
class DatabaseException : public adt::Exception {
public:
   /**
      Constructor.
   
      @param resultCode Clase utilizada para transferir codigo de error entre el gestor de base de datos
      y nuestro programa. Entre otra informacion contiene el error ocurrido en la ultima operacion realizada.
      @param fromFile Fichero en el que se provoco la situacion de error.
      @param fromLine Linea del fichero en la que se provoco la situacion de error.
   */
   DatabaseException (const ResultCode& resultCode, const char* fromMethod, const char* fromFile, const unsigned fromLine) :
      adt::Exception (resultCode.getErrorText(), fromMethod, fromFile, fromLine),
      m_resultCode (resultCode)
   {}
   
   /**
      Constructor.
   
      @param logicalName Nombre logico del elemento que genera la excepcion.
      @param resultCode Clase utilizada para transferir codigo de error entre el gestor de base de datos
      y nuestro programa. Entre otra informacion contiene el error ocurrido en la ultima operacion realizada.
      @param fromFile Fichero en el que se provoco la situacion de error.
      @param fromLine Linea del fichero en la que se provoco la situacion de error.
   */
   DatabaseException (const std::string& logicalName, const ResultCode& resultCode, const char* fromMethod, const char* fromFile, const int fromLine) :
      adt::Exception (std::string (logicalName).append (": ").append (resultCode.asString ()), fromMethod, fromFile, fromLine),
      m_resultCode (resultCode)
   {}

   DatabaseException (const DatabaseException& other) :
      Exception (other),
      m_resultCode (other.m_resultCode)
   {}

   /**
      Destructor.
   */
   ~DatabaseException () noexcept {;}
   
   /**
      Devuelve el resultado de base de datos asociado a la excepcion   
   
      @return El resultado de base de datos asociado a la excepcion   
   */   
   const ResultCode& getResultCode () const noexcept { return m_resultCode; }
   
   std::string asString () const noexcept;

private:
   const ResultCode m_resultCode;
};

}
}

#define COFFEE_THROW_DB_EXCEPTION(rc) do { throw coffee::dbms::DatabaseException (rc, __PRETTY_FUNCTION__, __FILE__, __LINE__); } while (false)
#define COFFEE_THROW_NAME_DB_EXCEPTION(logicalName, rc) do { throw coffee::dbms::DatabaseException (logicalName, rc, __PRETTY_FUNCTION__, __FILE__, __LINE__); } while (false)

#endif

