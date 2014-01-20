// WEPA - Write Excellent Professional Applications
#ifndef _wepa_dbms_datatype_Integer_h
#define _wepa_dbms_datatype_Integer_h

#include <wepa/dbms/datatype/Abstract.hpp>

namespace wepa {

namespace dbms {

namespace datatype {

/**
   Cadena usada como entrada y/o salida de las sentencias SQL.

   @author cisco.tierra@gmail.com.
*/
class Integer : public datatype::Abstract {
public:
   explicit Integer (const char* name, const Constraint::_v constraint = Constraint::CanNotBeNull);
   explicit Integer (const std::string& name, const Constraint::_v constraint = Constraint::CanNotBeNull);
   Integer (const Integer& other);

   int getValue () const throw (adt::RuntimeException) { this->exceptionWhenIsNull(); return m_value; }

   void setValue (const int i)
      noexcept
   {
      m_value = i;
      this->isNotNull();
   }

   Abstract* clone () const noexcept { return new Integer (*this); }

   operator adt::StreamString () const noexcept { return asString (); }

   adt::StreamString asString () const noexcept;

   static const char* className () noexcept { return "dbms::datatype::Integer"; }

   wepa_declare_datatype_downcast(Integer)

private:
   int m_value;

   void do_clear () noexcept { m_value = 0; }
   int do_compare (const Abstract& other) const throw (adt::RuntimeException);
};

}
}
}

#endif

