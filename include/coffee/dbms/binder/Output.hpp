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

protected:
   Output(std::shared_ptr<datatype::Abstract>& value) : Binder(value) {;}

private:
   void do_encode(Statement& statement, const int pos) throw(adt::RuntimeException) {;}
};

}
}
}

#endif

