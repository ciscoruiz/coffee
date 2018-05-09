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

#ifndef _coffee_dbms_datatype_Float_h
#define _coffee_dbms_datatype_Float_h

#include <coffee/dbms/datatype/Abstract.hpp>

namespace coffee {

namespace dbms {

namespace datatype {

/**
   Numero en coma flotante usado como entrada y/o salida de las sentencias SQL.

   @author frr@tid.es cisco.tierra@gmail.com.
   \since NemesisRD.dbms 1.5.4
*/
class Float : public datatype::Abstract {
public:
   explicit Float (const char* name, const Constraint::_v constraint = Constraint::CanNotBeNull, const char* format="%f");
   explicit Float (const std::string& name, const Constraint::_v constraint = Constraint::CanNotBeNull, const char* format="%f") : 
      Float(name.c_str(), constraint, format){;}
   Float (const Float& other);

   static std::shared_ptr<Float> instantiate(const char* name, const Constraint::_v constraint = Constraint::CanNotBeNull, const char* format="%f") {
      return std::make_shared<Float>(name, constraint, format);
   }

   float getFloatValue () const throw (basis::RuntimeException) { return getValue (); }

   float getValue () const throw (basis::RuntimeException) { this->exceptionWhenIsNull(); return m_value; }

   const char* getFormat () const noexcept { return m_format; }

   void setValue (const float value) noexcept {
      m_value = value;
      isNotNull();
   }

   std::shared_ptr<Abstract> clone() const noexcept { return std::make_shared<Float>(*this); }

   bool operator==(const Float& other) const noexcept { return m_value == other.m_value; }

   operator basis::StreamString () const noexcept { return asString (); }

   basis::StreamString asString () const noexcept;

   size_t hash() const noexcept { return std::hash<float>{}(m_value); }

   coffee_declare_datatype_downcast(Float);

private:
   float m_value;
   const char* m_format;

   void do_clear () noexcept { m_value = 0.0; }
   int do_compare (const Abstract& other) const throw (basis::RuntimeException);
};

}
}
}

#endif

