// WEPA - Write Excellent Professional Applications
#ifndef _wepa_dbms_datatype_Integer_h
#define _wepa_dbms_datatype_Integer_h

#include <wepa/dbms/datatype/Abstract.hpp>

namespace wepa {

namespace dbms {

namespace datatype {

/**
   Cadena usada como entrada y/o salida de las sentencias SQL.

   @author cisco.tierra@gmail.com.
*/
class Integer : public datatype::Abstract {
public:
   /**
      Constructor.
      \param name Nombre l�gico de este miembro.
      \param constraint Indica si el dato puede tomar valores nulos
   */
   explicit Integer (const char* name, const Constraint::_v constraint = Constraint::CanNotBeNull) :
      datatype::Abstract (name, Datatype::Integer, sizeof (int), constraint),
      m_value (0)
   {
      datatype::Abstract::setBuffer (&m_value);
   }

   /**
      Constructor.
      \param name Nombre l�gico de este miembro.
      \param constraint Indica si el dato puede tomar valores nulos

      \since NemesisRD.dbms 2.10.06.3
   */
   explicit Integer (const std::string& name, const Constraint::_v constraint = Constraint::CanNotBeNull) :
      datatype::Abstract (name, Datatype::Integer, sizeof (int), constraint),
      m_value (0)
   {
      datatype::Abstract::setBuffer (&m_value);
   }

   /**
      Constructor copia.
      \param other Instancia de la que copiar.
      \since NemesisRD.dbms 1.1.1
   */
   Integer (const Integer& other) :
      datatype::Abstract (other),
      m_value (other.m_value)
   {
      datatype::Abstract::setBuffer (&m_value);
   }

   /**
      Devuelve el valor entero asociado a esta instancia.
      \return El valor entero asociado a esta instancia.
   */
   int getValue () const throw (adt::RuntimeException) { this->exceptionWhenIsNull(); return m_value; }

   /**
      Operador de asignacin entero.
      \param i Valor entero a asignar.
      \return La referencia a esta instancia.
   */
   void setValue (const int i)
      noexcept
   {
      m_value = i;
      this->isNotNull();
   }

   /**
      Operador de conversion.
      \return El valor entero asociado a esta instancia.
   */
   operator int () const throw (adt::RuntimeException) { return getValue(); }

   /**
      Devuelve una cadena con la informacion referente a esta instancia.
      @return Una cadena con la informacion referente a esta instancia.
   */
   adt::StreamString asString () const noexcept;

   /**
    * Devuelve el nombre l�gico de esta clase
    * \return el nombre l�gico de esta clase
    * \since NemesisRD.dbms 2.10.16.04
    */
   static const char* className () noexcept { return "dbms::datatype::Integer"; }

   wepa_declare_datatype_downcast(Integer)

private:
   int m_value;

   void do_clear () noexcept { m_value = 0; }
};

}
}
}

#endif

