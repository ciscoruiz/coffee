// WEPA - Write Excellent Professional Applications
#ifndef _wepa_dbms_datatype_String_h
#define _wepa_dbms_datatype_String_h

#include <wepa/adt/RuntimeException.hpp>
#include <wepa/config/defines.hpp>

#include <wepa/dbms/datatype/Abstract.hpp>

namespace wepa {

namespace dbms {

namespace datatype {

/**
   Cadena usada como entrada y/o salida de las sentencias SQL.

   @author frr@tid.es cisco.tierra@gmail.com.
*/
class String : public datatype::Abstract {
public:
   /**
      Constructor.
      \param name Nombre l�gico de este miembro.
      \param maxSize Tamao maximo que puede tener esta cadena. Deberia coincidir con el indicado
      por la columna con la que vaya a corresponder en la sentencia.
      \param isNulleable Indica si el dato puede tomar valores nulos.
   */
   explicit String (const char* name, const int maxSize, const bool isNulleable = false) :
      datatype::Abstract (name, Datatype::String, maxSize, isNulleable)
   {
      datatype::Abstract::setBuffer (m_value = new char [maxSize + 1]);
      wepa_memset (m_value, 0, maxSize + 1);
   }

   /**
      Constructor.
      \param name Nombre l�gico de este miembro.
      \param maxSize Tamao maximo que puede tener esta cadena. Deberia coincidir con el indicado
      por la columna con la que vaya a corresponder en la sentencia.
      \param isNulleable Indica si el dato puede tomar valores nulos.

      \since NemesisRD.dbms 2.10.06.3
   */
   explicit String (const std::string& name, const int maxSize, const bool isNulleable = false) :
      datatype::Abstract (name, Datatype::String, maxSize, isNulleable)
   {
      datatype::Abstract::setBuffer (m_value = new char [maxSize + 1]);
      wepa_memset (m_value, 0, maxSize + 1);
   }

   /**
      Constructor copia.
      \param other Instancia de la que copiar.
      \since NemesisRD.dbms 1.1.1
   */
   String (const String& other) :
      datatype::Abstract (other),
      m_value (other.m_value)
   {
      const int maxSize = getMaxSize ();
      datatype::Abstract::setBuffer (m_value = new char [maxSize + 1]);
      wepa_memset (m_value, 0, maxSize + 1);
   }

   /**
      Destructor.
   */
   ~String () { delete [] m_value; }

   int getSize () const throw () { return (hasValue () == true) ? wepa_strlen(m_value): 0; }

   /**
      Devuelve el contenido de la cadena.
      \return El contenido de la cadena.
      \warning Si el metodo datatype::Abstract::isNull devolvio \em true el resultado de este metodo no esta definido.
   */
   const char* getValue () const throw (adt::RuntimeException) { this->exceptionWhenIsNull(); return m_value; }

   /**
      Operador de asignacin.
      \param str Cadena de la que copiar. Si la longitud de la cadena sobrepasa el tamao maximo
      indicado en el constructor obtendremos una excepcin.
      \return La instancia de esta cadena.
   */
   void setValue (const char* str) throw (adt::RuntimeException);

   /**
      Operador de asignacin.
      \param str Cadena de la que copiar. Si la longitud de la cadena sobrepasa el tamao maximo
      indicado en el constructor obtendremos una excepcin.
      \return La instancia de esta cadena.
   */
   void setValue (const std::string& str) throw (adt::RuntimeException) { return setValue (str.c_str ()); }

   /**
      Operador de conversion. Desde NemesisRD.dbms 1.5.3 si el contenido de la columna sociada
      fue nulo este metodo devolvera NULL.
      \return El contenido de esta cadena.
   */
   operator const char* () const throw (adt::RuntimeException) { this->exceptionWhenIsNull(); return  m_value; }

   /**
      Elimina los espacios a la derecha de la cadena recibida.
      \return La misma cadena recibida pero con los espacios eliminados.
   */
   static char* strip (char *str) throw ();

   /**
      Devuelve una cadena con la informacion referente a esta instancia.
      \return Una cadena con la informacion referente a esta instancia.
   */
   virtual adt::StreamString asString () const throw ();

   /**
    * Devuelve el nombre l�gico de esta clase
    * \return el nombre l�gico de esta clase
    */
   static const char* className () throw () { return "dbms::datatype::String"; }

private:
   char* m_value;

   void do_clear () throw () { m_value [0] = 0; }
};

}
}
}

#endif

