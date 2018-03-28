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

#ifndef _coffee_dbms_datatype_TimeStamp_h
#define _coffee_dbms_datatype_TimeStamp_h

#include <coffee/dbms/datatype/Date.hpp>

namespace coffee {

namespace dbms {

namespace datatype {

/**
   Tipo de datos que permite trabajar con el tipo de dato 'TimeStamp' de un gestor de base de
   datos generico.

   El tipo de dato TimeStamp contiene la informaci�n suficiente para representar una fecha
   incluyendo la hora del d�a.

   \author cisco.tierra@gmail.com.
   \since NemesisRD.dbms 1.6.1
*/
class TimeStamp : public Date {
public:
   explicit TimeStamp(const char* name, const Constraint::_v constraint = Constraint::CanNotBeNull);
   explicit TimeStamp(const std::string& name, const Constraint::_v constraint = Constraint::CanNotBeNull) :TimeStamp(name.c_str(), constraint) {;}

   TimeStamp(const datatype::TimeStamp& other);

   static std::shared_ptr<TimeStamp> instantiate(const char* name, const Constraint::_v constraint = Constraint::CanNotBeNull) {
      return std::make_shared<TimeStamp>(name, constraint);
   }

   std::shared_ptr<Abstract> clone() const noexcept { return std::make_shared<TimeStamp>(*this); }

   coffee_declare_datatype_downcast(TimeStamp);

private:
   int m_fractionalSecond;

   void do_clear() noexcept { Date::do_clear(); m_fractionalSecond = 0; }

};

}
}
}

#endif

