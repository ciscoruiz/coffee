// COFFEE - COmpany eFFEEctive Platform
#ifndef _coffee_dbms_bind_Input_h
#define _coffee_dbms_bind_Input_h

#include "Binder.hpp"

namespace coffee {

namespace dbms {

namespace binder {

/**
 * Clase que facilita la interconexion entre las variables del ambito C++ y el ambito RDBMS.
 *
 * @author frr@tid.es cisco.tierra@gmail.com.
 *
 * A continuacion presentamos un ejemplo de uso detallado.
 * \include dbms.ss/oracle.ss/db_null.p/db_null.cc
 */
class Input : public Binder {
public:
   adt::StreamString asString() const noexcept;

protected:
   explicit Input(std::shared_ptr<datatype::Abstract>& value) : Binder(value) {;}

private:
   void do_decode(Statement& statement, const int pos) throw(adt::RuntimeException) {}
};

}
}
}

#endif

