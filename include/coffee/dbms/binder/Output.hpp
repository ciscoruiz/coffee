// COFFEE - COmpany eFFEEctive Platform
#ifndef _coffee_dbms_binder_Output_h
#define _coffee_dbms_binder_Output_h

#include "Binder.hpp"

namespace coffee {

namespace dbms {

namespace datatype {
   class LongBlock;
}

namespace binder {

/**
   Clase que facilita la interconexion entre las variables del ambito C++ y el ambito RDBMS.

   @author frr@tid.es cisco.tierra@gmail.com.

   \since NemesisRD.dbms 1.4.2.

   A continuacion presentamos un ejemplo de uso detallado.
   \include dbms.ss/oracle.ss/db_null.p/db_null.cc
*/
class Output : public Binder {
public:
   adt::StreamString asString() const noexcept;

   /**
      Graba el valor de la variable coffee::dbms::LongBlock asociada a esta Output. Cualquier
      modificacion que necesitemos aplicar sobre la columnna de tipo se debera hacer mediante los
      metodos ofrecidos por la clase coffee::dbms::LongBlock.

      \warning Este metodo solo puede ser usado para variables de tipo dbms::Data::Type::LongBlock y
      siempre y cuando hayamos abierto el BLOB con una sentencia SQL de seleccion.
   */
   void write() throw(adt::RuntimeException, dbms::DatabaseException);

protected:
   Output(std::shared_ptr<datatype::Abstract>& value) : Binder(value) {;}

private:
   void do_encode() throw(adt::RuntimeException) {;}

   virtual void do_write(const std::shared_ptr<datatype::LongBlock>&) throw(adt::RuntimeException, dbms::DatabaseException) = 0;
};

}
}
}

#endif

