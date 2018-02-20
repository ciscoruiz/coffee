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

   int getValue() const throw(adt::RuntimeException) { this->exceptionWhenIsNull(); return m_value; }

   void setValue(const int i)
      noexcept
   {
      m_value = i;
      this->isNotNull();
   }

   std::shared_ptr<Abstract> clone() const noexcept { return std::make_shared<Integer>(*this); }

   bool operator==(const Integer& other) const noexcept { return m_value == other.m_value; }

   operator adt::StreamString() const noexcept { return asString(); }

   adt::StreamString asString() const noexcept;

   size_t hash() const noexcept { return std::hash<int>{}(m_value); }

   coffee_declare_datatype_downcast(Integer)

private:
   int m_value;

   void do_clear() noexcept { m_value = 0; }
   int do_compare(const Abstract& other) const throw(adt::RuntimeException);
};

}
}
}

#endif

