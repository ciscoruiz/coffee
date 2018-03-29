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

#ifndef _coffee_dbms_datatype_ShortBlock_h
#define _coffee_dbms_datatype_ShortBlock_h

#include <string>

#include <coffee/basis/DataBlock.hpp>
#include <coffee/basis/StreamString.hpp>

#include <coffee/dbms/datatype/Abstract.hpp>

namespace coffee {

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
   explicit ShortBlock (const char* name, const int maxSize, const Constraint::_v constraint = Constraint::CanNotBeNull);
   explicit ShortBlock (const std::string& name, const int maxSize, const Constraint::_v constraint = Constraint::CanNotBeNull) : 
      ShortBlock(name.c_str(), maxSize, constraint) {;}

   ShortBlock (const ShortBlock& other);
   ~ShortBlock () {;}

   static std::shared_ptr<ShortBlock> instantiate(const char* name, const int maxSize, const Constraint::_v constraint = Constraint::CanNotBeNull) {
      return std::make_shared<ShortBlock>(name, maxSize, constraint);
   }

   int getSize () const noexcept { return (hasValue () == true) ? m_value.size (): 0; }

   const basis::DataBlock& getValue () const throw (basis::RuntimeException) {  this->exceptionWhenIsNull();return m_value; }

   void setValue (const basis::DataBlock& value) throw (basis::RuntimeException);

   std::shared_ptr<Abstract> clone() const noexcept { return std::make_shared<ShortBlock>(*this); }

   operator basis::StreamString () const noexcept { return asString (); }

   basis::StreamString asString () const noexcept;

   size_t hash() const noexcept { return m_value.size(); }

   coffee_declare_datatype_downcast(ShortBlock);

protected:
   basis::DataBlock m_value;

   void do_clear () noexcept { m_value.clear (); }
   int do_compare (const Abstract& other) const throw (basis::RuntimeException);
};

}
}
}

#endif

