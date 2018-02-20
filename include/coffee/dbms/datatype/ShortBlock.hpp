#ifndef _coffee_dbms_datatype_ShortBlock_h
#define _coffee_dbms_datatype_ShortBlock_h

#include <string>

#include <coffee/adt/DataBlock.hpp>
#include <coffee/adt/StreamString.hpp>

#include <coffee/dbms/datatype/Abstract.hpp>

namespace coffee {

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
   explicit ShortBlock (const std::string& name, const int maxSize, const Constraint::_v constraint = Constraint::CanNotBeNull) : 
      ShortBlock(name.c_str(), maxSize, constraint) {;}

   ShortBlock (const ShortBlock& other);
   ~ShortBlock () {;}

   int getSize () const noexcept { return (hasValue () == true) ? m_value.size (): 0; }

   const adt::DataBlock& getValue () const throw (adt::RuntimeException) {  this->exceptionWhenIsNull();return m_value; }

   void setValue (const adt::DataBlock& value) throw (adt::RuntimeException);

   std::shared_ptr<Abstract> clone() const noexcept { return std::make_shared<ShortBlock>(*this); }

   operator adt::StreamString () const noexcept { return asString (); }

   adt::StreamString asString () const noexcept;

   size_t hash() const noexcept { return m_value.size(); }

   coffee_declare_datatype_downcast(ShortBlock)

protected:
   adt::DataBlock m_value;

   void do_clear () noexcept { m_value.clear (); }
   int do_compare (const Abstract& other) const throw (adt::RuntimeException);
};

}
}
}

#endif

