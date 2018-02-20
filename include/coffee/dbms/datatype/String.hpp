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

#ifndef _coffee_dbms_datatype_String_h
#define _coffee_dbms_datatype_String_h

#include <coffee/adt/RuntimeException.hpp>
#include <coffee/config/defines.hpp>

#include <coffee/dbms/datatype/Abstract.hpp>

namespace coffee {

namespace dbms {

namespace datatype {

/**
   Cadena usada como entrada y/o salida de las sentencias SQL.

   @author frr@tid.es cisco.tierra@gmail.com.
*/
class String : public datatype::Abstract {
public:
   explicit String(const char* name, const int maxSize, const Constraint::_v constraint = Constraint::CanNotBeNull);
   explicit String(const std::string& name, const int maxSize, const Constraint::_v constraint = Constraint::CanNotBeNull) : 
      String(name.c_str(), maxSize, constraint) {;}
   String(const String& other);
   ~String() { delete [] m_value; }

   int getSize() const noexcept { return(hasValue() == true) ? coffee_strlen(m_value): 0; }

   const char* getValue() const throw(adt::RuntimeException) { this->exceptionWhenIsNull(); return m_value; }

   void setValue(const char* str) throw(adt::RuntimeException);

   void setValue(const std::string& str) throw(adt::RuntimeException) { setValue(str.c_str()); }

   static char* strip(char *str) noexcept;

   std::shared_ptr<Abstract> clone() const noexcept { return std::make_shared<String>(*this); }

   bool operator==(const String& other) const noexcept { return coffee_strcmp (this->m_value, other.m_value) == 0; }

   operator adt::StreamString() const noexcept { return asString(); }

   adt::StreamString asString() const noexcept;

   size_t hash() const noexcept { return std::hash<std::string>{}(m_value); }

   coffee_declare_datatype_downcast(String)

private:
   char* m_value;

   void do_clear() noexcept { m_value [0] = 0; }
   int do_compare(const Abstract& other) const throw(adt::RuntimeException);
};

}
}
}

#endif

