// WEPA - Write Excellent Professional Applications
#ifndef _wepa_dbms_datatype_ShortBlock_h
#define _wepa_dbms_datatype_ShortBlock_h

#include <string>

#include <wepa/adt/DataBlock.hpp>
#include <wepa/adt/StreamString.hpp>

#include <wepa/dbms/datatype/Abstract.hpp>

namespace wepa {

namespace dbms {

namespace datatype {

/**
   Bloque de datos  usado como entrada y/o salida de las sentencias SQL.

   La longitud del dato a tratar estara en 2048 y 4096 bytes, dependiendo
   del RDBMS concreto con el que estemos trabajando.

   @author cisco.tierra@gmail.com.
   \see LongBlock
*/
class ShortBlock : public datatype::Abstract {
public:
   /**
      Constructor.
      \param name Nombre l�gico de este miembro.
      \param maxSize Tamao maximo que puede tener este bloque.
      \param isNulleable Indica si el dato puede tomar valores nulos.
   */
   explicit ShortBlock (const char* name, const int maxSize, const bool isNulleable = false) :
      datatype::Abstract (name, Datatype::ShortBlock, maxSize, isNulleable),
      m_value ()
   {
      m_value.reserve(maxSize);
      datatype::Abstract::setBuffer ((void*) m_value.data ());
   }

   /**
      Constructor.
      \param name Nombre l�gico de este miembro.
      \param maxSize Tamao maximo que puede tener este bloque.
      \param isNulleable Indica si el dato puede tomar valores nulos.

      \since NemesisRD.dbms 2.10.06.3
   */
   explicit ShortBlock (const std::string& name, const int maxSize, const bool isNulleable = false) :
      datatype::Abstract (name, Datatype::ShortBlock, maxSize, isNulleable),
      m_value ()
   {
      m_value.reserve(maxSize);
      datatype::Abstract::setBuffer ((void*) m_value.data ());
   }

   /**
      Constructor copia.
      \param other Instancia de la que copiar.
      \since NemesisRD.dbms 1.1.1
   */
   ShortBlock (const ShortBlock& other) :
      datatype::Abstract (other),
      m_value ()
   {
      m_value = other.m_value;
      datatype::Abstract::setBuffer ((void*) m_value.data ());
   }

   /**
      Destructor.
   */
   virtual ~ShortBlock () {;}

   /**
      Devuelve el tamao actual de este dato.
      \return El tamao actual de este dato.
   */
   int getSize () const throw () { return m_value.size (); }

   /**
      Devuelve el contenido de la este bloque de memoria.
      \return  Devuelve el contenido de la este bloque de memoria.
      \warning Si el metodo datatype::Abstract::isNull devolvio \em true el resultado de este metodo no esta definido.
   */
   const adt::DataBlock& getValue () const throw () { return m_value; }

   /**
      Operador de asignacin.
      \param other Bloque del que copiar.
      \return La instancia de este bloque de memoria.
   */
   ShortBlock& operator = (const ShortBlock& other) throw (adt::RuntimeException);

   /**
      Operador de asignacin.
      \param value Valor que queremos a asignar.
      \return La instancia de esta cadena.
   */
   ShortBlock& operator = (const adt::DataBlock& value) throw (adt::RuntimeException);

   /**
      Operador de conversion.
      \return El adt::DataBlock asociado a esta instancia.
   */
   operator adt::DataBlock& () throw () { return m_value; }

   /**
      Operador de conversion.
      \return El adt::DataBlock asociado a esta instancia.
   */
   operator const adt::DataBlock& () const throw () { return m_value; }

   /**
      Devuelve una cadena con la informacion referente a esta instancia.
      \return Una cadena con la informacion referente a esta instancia.
   */
   adt::StreamString asString () const throw ();

protected:
   adt::DataBlock m_value;
};

}
}
}

#endif

