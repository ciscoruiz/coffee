// WEPA - Write Excellent Professional Applications
#ifndef _wepa_dbms_datatype_TimeStamp_h
#define _wepa_dbms_datatype_TimeStamp_h

#include <wepa/dbms/datatype/Date.hpp>

namespace wepa {

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
   explicit TimeStamp(const std::string& name, const Constraint::_v constraint = Constraint::CanNotBeNull, const char* format=NULL);
   TimeStamp(const datatype::TimeStamp& other);

   int getFractionalSecond() const noexcept { return m_fractionalSecond; }

   void setFractionalSecond(const int fsec) noexcept { m_fractionalSecond = fsec; }

   std::shared_ptr<Abstract> clone() const noexcept { return std::make_shared<TimeStamp>(*this); }

   const char* className() noexcept { return "dbms::datatype::TimeStamp"; }

   size_t hash() const noexcept { return std::hash(m_value); }

   wepa_declare_datatype_downcast(TimeStamp)

private:
   char m_anotherBuffer [MaxDateSize + 1];
   int m_fractionalSecond;

   void do_clear() noexcept { Date::do_clear(); m_fractionalSecond = 0; m_anotherBuffer [0] = 0; }

};

}
}
}

#endif

