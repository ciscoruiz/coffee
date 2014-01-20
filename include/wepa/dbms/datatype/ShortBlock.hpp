// WEPA - Write Excellent Professional Applications
#ifndef _wepa_dbms_datatype_ShortBlock_h
#define _wepa_dbms_datatype_ShortBlock_h

#include <string>

#include <wepa/adt/DataBlock.hpp>
#include <wepa/adt/StreamString.hpp>

#include <wepa/dbms/datatype/Abstract.hpp>

namespace wepa {

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
   explicit ShortBlock (const std::string& name, const int maxSize, const Constraint::_v constraint = Constraint::CanNotBeNull);
   ShortBlock (const ShortBlock& other);
   ~ShortBlock () {;}

   int getSize () const noexcept { return (hasValue () == true) ? m_value.size (): 0; }

   const adt::DataBlock& getValue () const throw (adt::RuntimeException) {  this->exceptionWhenIsNull();return m_value; }

   void setValue (const adt::DataBlock& value) throw (adt::RuntimeException);

   Abstract* clone () const noexcept { return new ShortBlock(*this); }

   operator adt::StreamString () const noexcept { return asString (); }

   adt::StreamString asString () const noexcept;

   wepa_declare_datatype_downcast(ShortBlock)

protected:
   adt::DataBlock m_value;

   void do_clear () noexcept { m_value.clear (); }
   int do_compare (const Abstract& other) const throw (adt::RuntimeException);
};

}
}
}

#endif

