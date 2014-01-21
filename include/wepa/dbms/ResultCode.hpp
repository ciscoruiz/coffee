// WEPA - Write Excellent Professional Applications
#ifndef _wepm_dbms_ResultCode_h
#define _wepm_dbms_ResultCode_h

#include <string>

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
class ResultCode {
public:      
   /**
      Constructor vacio.
      \warning Antes de usarse debe asignarse a algun otro ResultCode obtenido mediante la invocacion
      a wepa::dbms::Connection::execute.
   */
   ResultCode () : m_errorText (NULL), m_database (NULL), m_numericCode (0) {;}

   /**
      Constructor vacio.
      \warning Antes de usarse debe asignarse a algun otro ResultCode obtenido mediante la invocacion
      a wepa::dbms::Connection::execute.
   */
   explicit ResultCode (const Database& database, const int opCode) : m_errorText (NULL), m_database (&database), m_numericCode (opCode) {;}

   /**
      Constructor copia.      
      @param other Instancia de la que copiar los datos.
   */
   ResultCode (const ResultCode& other)  :
      m_errorText (NULL),
      m_database (other.m_database)
   {
      initialize (other.m_numericCode, other.m_errorText);
   }   
   
   /**
      Destructor.
   */
   virtual ~ResultCode () { if (m_errorText != NULL) free (m_errorText); }

   void initialize (const int numericCode, const char* errorText)
      noexcept
   {
      m_numericCode = numericCode;
      copy (errorText);
   }

   /**
      Devuelve el codigo de error del ultimo comando ejecutado contra la base de datos.   
      @return El codigo de error del ultimo comando ejecutado contra la base de datos.
   */   
   int getNumericCode () const noexcept { return m_numericCode; }
   
   /**
      Devuelve el texto del error del ultimo comando ejecutado contra la base de datos.
      @return El texto del error del ultimo comando ejecutado contra la base de datos.
   */
   const char* getErrorText () const noexcept { return (m_errorText != NULL) ? m_errorText: ""; }

   // Operadores
   /**
      Operador copia.      
      @param resultCode Instancia a copiar.
      @return Una instancia de si mismo.
   */      
   ResultCode& operator = (const ResultCode& resultCode) 
      noexcept
   {
      if (this != &resultCode) {
         m_database = resultCode.m_database;
         initialize (resultCode.m_numericCode, resultCode.m_errorText);
      }
   
      return *this;
   }
   
   /**
      Devuelve \em true si las condiciones de busqueda de la ultimo operacion
      no han sido satisfechas por ningun registro o \em false en otro caso.      
      @return \em true si las condiciones de busqueda de la ultimo operacion
      no han sido satisfechas por ningun registro o \em false en otro caso.
   */   
   bool notFound () const throw (adt::RuntimeException);
   
   /**
      Devuelve \em true si la ultima operacion solicitada fue realizada correctamente
      o \em false en otro caso.      
      @return \em true si la ultima operacion solicitada fue realizada correctamente
      o \em false en otro caso.      
   */
   bool successful () const throw (adt::RuntimeException);

   /**
      Devuelve \em true Si el registro obtenenido en una sentencia de seleccion con indicador
      de modo exclusivo ha sido bloqueada previamente por otro proceso y/o contexto de base de 
      datos o \em false en otro caso.      
      @return \em true Si el registro obtenenido en una sentencia de seleccion con indicador
      de modo exclusivo ha sido bloqueada previamente por otro proceso y/o contexto de base de 
      datos o \em false en otro caso.      
   */
   bool locked () const throw (adt::RuntimeException);

   /**
      Devuelve \em true si se perdio la conexion la base de datos o \em false en otro caso.
      @return \em true si se perdio la conexion la base de datos o \em false en otro caso.
   */
   bool lostConnection () const throw (adt::RuntimeException);

   operator adt::StreamString () const noexcept { return asString (); }

   /**
      Devuelve una cadena con la informacion sobre esta clase.
      \return Una cadena con la informacion sobre esta clase.
   */
   adt::StreamString asString () const noexcept;

protected:
   static const int MaxErrorLen = 512;

   /**
      Constructor.
      
      \param numericCode Codigo de error asociado a la ultima operacion realizada contra la base de datos.
      \param errorText Texto asociado al error de ultima operacion realizada contra la base de datos. Puede ser
      NULL si no hay ningun texto de error asociado al codigo recibido.
      \param database Decofidicador de errores.
   */
   ResultCode (const int numericCode, const char* errorText, const Database& database) :
      m_errorText (NULL),
      m_database (&database)
   {
      initialize (numericCode, errorText);
   }

   /**
      Establece el contenido de esta clase.
   
      \param numericCode Codigo de error asociado a la ultima operacion realizada contra la base de datos.
      \param errorText Texto asociado al error de ultima operacion realizada contra la base de datos.
   */   

private:
   int m_numericCode;
   char* m_errorText;
   const Database* m_database;
   
   void copy (const char* text) noexcept;
};

}
}

#endif

