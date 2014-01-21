// WEPA - Write Excellent Professional Applications
#ifndef _wepa_dbms_datatype_Float_h
#define _wepa_dbms_datatype_Float_h

#include <wepa/dbms/datatype/Abstract.hpp>

namespace wepa {

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
   explicit Float (const std::string& name, const Constraint::_v constraint = Constraint::CanNotBeNull, const char* format="%f");
   Float (const Float& other);

   float getFloatValue () const throw (adt::RuntimeException) { return getValue (); }

   float getValue () const throw (adt::RuntimeException) { this->exceptionWhenIsNull(); return m_value; }

   const char* getFormat () const noexcept { return m_format; }

   void setValue (const float value) noexcept {
      m_value = value;
      isNotNull();
   }

   Abstract* clone () const noexcept { return new Float (*this); }

   operator adt::StreamString () const noexcept { return asString (); }

   adt::StreamString asString () const noexcept;

   static const char* className () noexcept { return "dbms::datatype::Float"; }

   wepa_declare_datatype_downcast(Float)

private:
   float m_value;
   const char* m_format;

   void do_clear () noexcept { m_value = 0.0; }
   int do_compare (const Abstract& other) const throw (adt::RuntimeException);
};

}
}
}

#endif

