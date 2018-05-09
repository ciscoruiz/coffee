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

#ifndef _coffee_dbms_ResultCode_h
#define _coffee_dbms_ResultCode_h

#include <string>
#include <memory>

#include <string.h>

#include <stdlib.h>

#include <coffee/basis/RuntimeException.hpp>

namespace coffee {

namespace dbms {

class Database;
class ErrorCodeInterpreter;


/**
   Clase para acceder a la informacion devuelta por el gestor de base de datos
   referente al ultimo comando realizado.

   @author cisco.tierra@gmail.com
 */
class ResultCode {
public:      
   /**
      Constructor vacio.
      \warning Antes de usarse debe asignarse a algun otro ResultCode obtenido mediante la invocacion
      a coffee::dbms::Connection::execute.
   */
   ResultCode() : m_errorCodeInterpreter(), m_numericCode(0) {;}

   /**
      Constructor vacio.
      \warning Antes de usarse debe asignarse a algun otro ResultCode obtenido mediante la invocacion
      a coffee::dbms::Connection::execute.
   */
   explicit ResultCode(const Database& database, const int opCode);

   /**
      Constructor.

      \param numericCode Codigo de error asociado a la ultima operacion realizada contra la base de datos.
      \param errorText Texto asociado al error de ultima operacion realizada contra la base de datos. Puede ser
      NULL si no hay ningun texto de error asociado al codigo recibido.
      \param database Decofidicador de errores.
   */
   explicit ResultCode(const Database& database, const int numericCode, const char* errorText);


   /**
      Constructor copia.      
      @param other Instancia de la que copiar los datos.
   */
   ResultCode(const ResultCode& other)  :
      m_errorCodeInterpreter(other.m_errorCodeInterpreter),
      m_numericCode(other.m_numericCode),
      m_errorText(other.m_errorText)

   {
   }   
   
   /**
      Destructor.
   */
   virtual ~ResultCode() { }

   /**
      Devuelve el codigo de error del ultimo comando ejecutado contra la base de datos.   
      @return El codigo de error del ultimo comando ejecutado contra la base de datos.
   */   
   int getNumericCode() const noexcept { return m_numericCode; }
   
   /**
      Devuelve el texto del error del ultimo comando ejecutado contra la base de datos.
      @return El texto del error del ultimo comando ejecutado contra la base de datos.
   */
   const std::string& getErrorText() const noexcept { return m_errorText; }
   
   ResultCode& operator =(const ResultCode& other)
       noexcept
    {
       if(this != &other) {
          m_errorCodeInterpreter = other.m_errorCodeInterpreter;
          m_numericCode = other.m_numericCode;
          m_errorText = other.m_errorText;
       }

       return *this;
    }

   /**
      Devuelve \em true si las condiciones de busqueda de la ultimo operacion
      no han sido satisfechas por ningun registro o \em false en otro caso.      
      @return \em true si las condiciones de busqueda de la ultimo operacion
      no han sido satisfechas por ningun registro o \em false en otro caso.
   */   
   bool notFound() const throw(basis::RuntimeException);
   
   /**
      Devuelve \em true si la ultima operacion solicitada fue realizada correctamente
      o \em false en otro caso.      
      @return \em true si la ultima operacion solicitada fue realizada correctamente
      o \em false en otro caso.      
   */
   bool successful() const throw(basis::RuntimeException);

   /**
      Devuelve \em true Si el registro obtenenido en una sentencia de seleccion con indicador
      de modo exclusivo ha sido bloqueada previamente por otro proceso y/o contexto de base de 
      datos o \em false en otro caso.      
      @return \em true Si el registro obtenenido en una sentencia de seleccion con indicador
      de modo exclusivo ha sido bloqueada previamente por otro proceso y/o contexto de base de 
      datos o \em false en otro caso.      
   */
   bool locked() const throw(basis::RuntimeException);

   /**
      Devuelve \em true si se perdio la conexion la base de datos o \em false en otro caso.
      @return \em true si se perdio la conexion la base de datos o \em false en otro caso.
   */
   bool lostConnection() const throw(basis::RuntimeException);

   /**
      Devuelve una cadena con la informacion sobre esta clase.
      \return Una cadena con la informacion sobre esta clase.
   */
   operator basis::StreamString() const noexcept { return asString(); }

   /**
      Devuelve una cadena con la informacion sobre esta clase.
      \return Una cadena con la informacion sobre esta clase.
   */
   basis::StreamString asString() const noexcept;

private:
   std::shared_ptr<ErrorCodeInterpreter> m_errorCodeInterpreter;
   int m_numericCode;
   std::string m_errorText;
};

}
}

#endif

