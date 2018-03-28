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

#ifndef _coffee_dbms_datatype_LongBlock_h
#define _coffee_dbms_datatype_LongBlock_h

#include <string>

#include <coffee/basis/DataBlock.hpp>

#include <coffee/dbms/datatype/Abstract.hpp>

namespace coffee {

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
   explicit LongBlock(const std::string& name, const Constraint::_v constraint = Constraint::CanNotBeNull) : LongBlock(name.c_str(), constraint) {;}
   LongBlock(const LongBlock& other);
   ~LongBlock() {;}

   static std::shared_ptr<LongBlock> instantiate(const char* name, const Constraint::_v constraint = Constraint::CanNotBeNull) {
      return std::make_shared<LongBlock>(name, constraint);
   }

   int getSize() const noexcept { return(hasValue() == true) ? m_value.size(): 0; }

   const basis::DataBlock& getValue(basis::RuntimeException) const noexcept { this->exceptionWhenIsNull(); return m_value; }

   basis::DataBlock& getValue() throw(basis::RuntimeException) { this->exceptionWhenIsNull(); return m_value; }

   void setValue(const basis::DataBlock& value) throw(basis::RuntimeException);

   std::shared_ptr<Abstract> clone() const noexcept { return std::make_shared<LongBlock>(*this); }

   operator basis::StreamString() const noexcept { return asString(); }

   basis::StreamString asString() const noexcept;
   
   size_t hash() const noexcept { return m_value.size(); }

   coffee_declare_datatype_downcast(LongBlock);

protected:
   basis::DataBlock m_value;

   void do_clear() noexcept { m_value.clear(); }
   int do_compare(const Abstract& other) const throw(basis::RuntimeException);
};

}
}
}

#endif

