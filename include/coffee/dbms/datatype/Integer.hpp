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

#ifndef _coffee_dbms_datatype_Integer_h
#define _coffee_dbms_datatype_Integer_h

#include <coffee/dbms/datatype/Abstract.hpp>

namespace coffee {

namespace dbms {

namespace datatype {

/**
   Cadena usada como entrada y/o salida de las sentencias SQL.

   @author cisco.tierra@gmail.com.
*/
class Integer : public datatype::Abstract {
public:
   explicit Integer(const char* name, const Constraint::_v constraint = Constraint::CanNotBeNull);
   explicit Integer(const std::string& name, const Constraint::_v constraint = Constraint::CanNotBeNull) : Integer(name.c_str(), constraint) {;}
   Integer(const Integer& other);

   int getValue() const throw(basis::RuntimeException) { this->exceptionWhenIsNull(); return m_value; }

   void setValue(const int i)
      noexcept
   {
      m_value = i;
      this->isNotNull();
   }

   std::shared_ptr<Abstract> clone() const noexcept { return std::make_shared<Integer>(*this); }

   bool operator==(const Integer& other) const noexcept { return m_value == other.m_value; }

   operator basis::StreamString() const noexcept { return asString(); }

   basis::StreamString asString() const noexcept;

   size_t hash() const noexcept { return std::hash<int>{}(m_value); }

   coffee_declare_datatype_downcast(Integer);

private:
   int m_value;

   void do_clear() noexcept { m_value = 0; }
   int do_compare(const Abstract& other) const throw(basis::RuntimeException);
};

}
}
}

#endif

