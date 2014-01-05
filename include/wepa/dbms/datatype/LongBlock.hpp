// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2013 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/wepa
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#ifndef _wepa_dbms_datatype_LongBlock_h
#define _wepa_dbms_datatype_LongBlock_h

#include <string>

#include <wepa/adt/DataBlock.hpp>

#include <wepa/dbms/datatype/Abstract.hpp>

namespace wepa {

namespace dbms {

namespace datatype {

/**
   Bloque de datos usado como entrada y/o salida de las sentencias SQL.

   A diferencia del tipo de datos ShortBlock, en principio, no tiene ninguna limitacion
   en cuanto a la longitud del campo que vamos a tratar. Por contra, dependiendo del motor
   de base de datos que vayamos a usar puede tener un tratamiento especial a la hora de
   grabarlo en la base de datos.

   @author cisco.tierra@gmail.com.
   \see ShortBlock
*/
class LongBlock : public datatype::Abstract {
public:
   /**
      Constructor.
      \param name Nombre l�gico de este miembro.
      \param isNulleable Indica si el dato puede tomar valores nulos.
   */
   explicit LongBlock (const char* name, const bool isNulleable = false) :
      datatype::Abstract (name, Datatype::LongBlock, 0, isNulleable),
      m_value ()
   {
      datatype::Abstract::setBuffer ((void*) NULL);
   }

   /**
      Constructor.
      \param name Nombre l�gico de este miembro.
      \param isNulleable Indica si el dato puede tomar valores nulos.

      \since NemesisRD.dbms 2.10.06.3
   */
   explicit LongBlock (const std::string& name, const bool isNulleable = false) :
      datatype::Abstract (name, Datatype::LongBlock, 0, isNulleable),
      m_value ()
   {
      datatype::Abstract::setBuffer ((void*) NULL);
   }

   /**
      Constructor copia.
      \param other Instancia de la que copiar.
      \since NemesisRD.dbms 1.1.1
   */
   LongBlock (const LongBlock& other) :
      datatype::Abstract (other),
      m_value ()
   {
      m_value = other.m_value;
   }

   /**
      Destructor.
   */
   virtual ~LongBlock () {;}

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
   const adt::DataBlock& getValue (adt::RuntimeException) const throw () { this->exceptionWhenIsNull(); return m_value; }

   /**
      Devuelve el contenido de la este bloque de memoria.
      \return  Devuelve el contenido de la este bloque de memoria.
      \warning Si el metodo datatype::Abstract::isNull devolvio \em true el resultado de este metodo no esta definido.
   */
   adt::DataBlock& getValue () throw (adt::RuntimeException) { this->exceptionWhenIsNull(); return m_value; }

   /**
      Operador de asignacin.
      \param value Valor que queremos a asignar.
      \return La instancia de esta cadena.
   */
   LongBlock& operator = (const adt::DataBlock& value) throw (adt::RuntimeException);

   /**
      Operador de conversion.
      \return El adt::DataBlock asociado a esta instancia.
   */
   operator adt::DataBlock& () throw (adt::RuntimeException) { this->exceptionWhenIsNull(); return m_value; }

   /**
      Operador de conversion.
      \return El adt::DataBlock asociado a esta instancia.
   */
   operator const adt::DataBlock& () const throw (adt::RuntimeException) { this->exceptionWhenIsNull(); return m_value; }

   /**
      Devuelve una cadena con la informacion referente a esta instancia.
      \return Una cadena con la informacion referente a esta instancia.
   */
   adt::StreamString asString () const throw ();

protected:
   adt::DataBlock m_value;

   void do_clear () throw () { m_value.clear (); }
};

}
}
}

#endif

