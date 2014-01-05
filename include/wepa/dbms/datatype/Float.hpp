// WEPA - Write Excellent Professional Applications
#ifndef _wepa_dbms_datatype_Float_h
#define _wepa_dbms_datatype_Float_h

#include <wepa/dbms/datatype/Abstract.hpp>

namespace wepa {

namespace dbms {

namespace datatype {

/**
   Numero en coma flotante usado como entrada y/o salida de las sentencias SQL.

   @author frr@tid.es cisco.tierra@gmail.com.
   \since NemesisRD.dbms 1.5.4
*/
class Float : public datatype::Abstract {
public:
   /**
      Constructor.
      \param name Nombre l�gico de este miembro.
      \param isNulleable Indica si el dato puede tomar valores nulos.
      \param format Indica el indicador de formato para pasar de cadena a numero. Usa la misma nomenclatura
      que printf, scanf, etc. Su uso depender� del gestor de base de datos usado.
   */
   explicit Float (const char* name, const bool isNulleable = false, const char* format="%f") :
      datatype::Abstract (name, Datatype::Float, sizeof (float), isNulleable),
      m_format (format),
      m_value (0.0)
   {
      datatype::Abstract::setBuffer (&m_value);
   }

   /**
      Constructor.
      \param name Nombre l�gico de este miembro.
      \param isNulleable Indica si el dato puede tomar valores nulos.
      \param format Indica el indicador de formato para pasar de cadena a numero. Usa la misma nomenclatura
      que printf, scanf, etc. Su uso depender� del gestor de base de datos usado.

      \since NemesisRD.dbms 2.10.06.3
   */
   explicit Float (const std::string& name, const bool isNulleable = false, const char* format="%f") :
      datatype::Abstract (name, Datatype::Float, sizeof (float), isNulleable),
      m_format (format),
      m_value (0.0)
   {
      datatype::Abstract::setBuffer (&m_value);
   }

   /**
      Constructor copia.
      \param other Instancia de la que copiar.
   */
   Float (const Float& other) : datatype::Abstract (other), m_value (other.m_value), m_format (other.m_format)
   {
      datatype::Abstract::setBuffer (&m_value);
   }

   /**
    * Metodo obsoleto desde NemesisRD.dbms 1.7.4, deber�a usar #getValue.
      Devuelve el contenido del campo de tipo Float.
      \return el contenido del campo de tipo Float.
      \warning Si el metodo datatype::Abstract::isNull devolvio \em true el resultado de este metodo no esta definido.
   */
   float getFloatValue () const throw (adt::RuntimeException) { return getValue (); }

   /**
    * Devuelve el valor asociado a este campo.
    * \return Devuelve el valor asociado a este campo.
    * \since NemesisRD.dbms 1.7.4.
    */
   float getValue () const throw (adt::RuntimeException) { this->exceptionWhenIsNull(); return m_value; }

   /**
    * Devuelve el formato que indica la forma en la que el n�mero ser� representado sobre
    * una cadena, en caso de que fuera necesario.
    * \since NemesisRD.dbms 1.7.4
    */
   const char* getFormat () const throw () { return m_format; }

   /**
      Operador de asignacion.
      \param value Float del que copiar.
      \return La instancia de esta cadena.
   */
   Float& operator = (const float value) throw () {
      m_value = value;
      isNotNull();
      return *this;
   }

   /**
      Operador de conversion.
      \warning Si la columna asociada tiene un valor NULL, devolvera 0.0.
      \return El contenido de esta cadena.
   */
   operator float () const throw (adt::RuntimeException) { return getValue (); }

   /**
      Devuelve una cadena con la informacion referente a esta instancia.
      \return Una cadena con la informacion referente a esta instancia.
   */
   adt::StreamString asString () const throw ();

   /**
    * Devuelve el nombre l�gico de esta clase
    * \return el nombre l�gico de esta clase
    * \since NemesisRD.dbms 2.10.16.04
    */
   static const char* className () throw () { return "dbms::datatype::Float"; }

private:
   float m_value;
   const char* m_format;

   void do_clear () throw () { m_value = 0.0; }
};

}
}
}

#endif

