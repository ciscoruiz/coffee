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

#ifndef _wepm_dbms_DatabaseException_hpp
#define _wepm_dbms_DatabaseException_hpp

#include <coffee/basis/Exception.hpp>

#include <coffee/dbms/ResultCode.hpp>

#include <coffee/basis/StreamString.hpp>

namespace coffee {

namespace dbms {

/**
   Exception ocurrida al acceder a algun servicio de la base de datos.
*/
class DatabaseException : public basis::Exception {
public:
   /**
      Constructor.
   
      @param resultCode Clase utilizada para transferir codigo de error entre el gestor de base de datos
      y nuestro programa. Entre otra informacion contiene el error ocurrido en la ultima operacion realizada.
      @param fromMethod Method which throws the exception.
      @param fromFile Fichero en el que se provoco la situacion de error.
      @param fromLine Linea del fichero en la que se provoco la situacion de error.
   */
   DatabaseException (const ResultCode& resultCode, const char* fromMethod, const char* fromFile, const unsigned fromLine) :
      basis::Exception (resultCode.getErrorText(), fromMethod, fromFile, fromLine),
      m_resultCode (resultCode)
   {}
   
   /**
      Constructor.
   
      @param logicalName Nombre logico del elemento que genera la excepcion.
      @param resultCode Clase utilizada para transferir codigo de error entre el gestor de base de datos
      y nuestro programa. Entre otra informacion contiene el error ocurrido en la ultima operacion realizada.
      @param fromMethod Method which throws the exception.
      @param fromFile Fichero en el que se provoco la situacion de error.
      @param fromLine Linea del fichero en la que se provoco la situacion de error.
   */
   DatabaseException (const std::string& logicalName, const ResultCode& resultCode, const char* fromMethod, const char* fromFile, const int fromLine) :
      basis::Exception (std::string (logicalName).append (": ").append (resultCode.asString ()), fromMethod, fromFile, fromLine),
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

