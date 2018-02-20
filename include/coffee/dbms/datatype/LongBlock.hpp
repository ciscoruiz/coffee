#ifndef _coffee_dbms_datatype_LongBlock_h
#define _coffee_dbms_datatype_LongBlock_h

#include <string>

#include <coffee/adt/DataBlock.hpp>

#include <coffee/dbms/datatype/Abstract.hpp>

namespace coffee {

namespace dbms {

namespace datatype {

/**
   Bloque de datos usado como entrada y/o salida de las sentencias SQL.

   A diferencia del tipo de datos ShortBlock, en principio, no tiene ninguna limitacion
   en cuanto a la longitud del campo que vamos a tratar. Por contra, dependiendo del motor
   de base de datos que vayamos a usar puede tener un tratamiento especial a la hora de
   grabarlo en la base de datos.

   @author cisco.tierra@gmail.com.
   \see ShortBlock
*/
class LongBlock : public datatype::Abstract {
public:
   explicit LongBlock(const char* name, const Constraint::_v constraint = Constraint::CanNotBeNull);
   explicit LongBlock(const std::string& name, const Constraint::_v constraint = Constraint::CanNotBeNull) : LongBlock(name.c_str(), constraint) {;}
   LongBlock(const LongBlock& other);
   ~LongBlock() {;}

   int getSize() const noexcept { return(hasValue() == true) ? m_value.size(): 0; }

   const adt::DataBlock& getValue(adt::RuntimeException) const noexcept { this->exceptionWhenIsNull(); return m_value; }

   adt::DataBlock& getValue() throw(adt::RuntimeException) { this->exceptionWhenIsNull(); return m_value; }

   void setValue(const adt::DataBlock& value) throw(adt::RuntimeException);

   std::shared_ptr<Abstract> clone() const noexcept { return std::make_shared<LongBlock>(*this); }

   operator adt::StreamString() const noexcept { return asString(); }

   adt::StreamString asString() const noexcept;
   
   size_t hash() const noexcept { return m_value.size(); }

   coffee_declare_datatype_downcast(LongBlock)

protected:
   adt::DataBlock m_value;

   void do_clear() noexcept { m_value.clear(); }
   int do_compare(const Abstract& other) const throw(adt::RuntimeException);
};

}
}
}

#endif

