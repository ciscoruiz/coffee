// COFFEE - COmpany eFFEEctive Platform
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

   float getFloatValue () const throw (adt::RuntimeException) { return getValue (); }

   float getValue () const throw (adt::RuntimeException) { this->exceptionWhenIsNull(); return m_value; }

   const char* getFormat () const noexcept { return m_format; }

   void setValue (const float value) noexcept {
      m_value = value;
      isNotNull();
   }

   std::shared_ptr<Abstract> clone() const noexcept { return std::make_shared<Float>(*this); }

   bool operator==(const Float& other) const noexcept { return m_value == other.m_value; }

   operator adt::StreamString () const noexcept { return asString (); }

   adt::StreamString asString () const noexcept;

   const char* className () noexcept { return "dbms::datatype::Float"; }

   size_t hash() const noexcept { return std::hash<float>{}(m_value); }

   coffee_declare_datatype_downcast(Float)

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

