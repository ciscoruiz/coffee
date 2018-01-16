// WEPA - Write Excellent Professional Applications
//
//(c) Copyright 2013 Francisco Ruiz Rayo
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
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
   explicit LongBlock(const char* name, const Constraint::_v constraint = Constraint::CanNotBeNull);
   explicit LongBlock(const std::string& name, const Constraint::_v constraint = Constraint::CanNotBeNull);
   LongBlock(const LongBlock& other);
   ~LongBlock() {;}

   int getSize() const noexcept { return(hasValue() == true) ? m_value.size(): 0; }

   const adt::DataBlock& getValue(adt::RuntimeException) const noexcept { this->exceptionWhenIsNull(); return m_value; }

   adt::DataBlock& getValue() throw(adt::RuntimeException) { this->exceptionWhenIsNull(); return m_value; }

   void setValue(const adt::DataBlock& value) throw(adt::RuntimeException);

   std::shared_ptr<Abstract> clone() const noexcept { return std::make_shared<LongBlock>(*this); }

   operator adt::StreamString() const noexcept { return asString(); }

   adt::StreamString asString() const noexcept;
   
   const char* className() noexcept { return "dbms::datatype::LongBlock"; }

   size_t hash() const noexcept { return std::hash(m_value); }

   wepa_declare_datatype_downcast(LongBlock)

protected:
   adt::DataBlock m_value;

   void do_clear() noexcept { m_value.clear(); }
   int do_compare(const Abstract& other) const throw(adt::RuntimeException);
};

}
}
}

#endif

