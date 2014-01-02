// WEPA - Write Excellent Professional Applications
#ifndef _wepa_dbms_bind_Input_h
#define _wepa_dbms_bind_Input_h

#include <wepa/dbms/binder/BinderIf.hpp>

namespace wepa {

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
class Input : public BinderIf {
public:
   adt::StreamString asString () const throw ();

protected:
   explicit Input (datatype::Abstract& value) : BinderIf (value) {;}

private:
   // �ste metodo no sera invocado nunca. A partir de un Input nunca hay que
   // convertir de C++ -> RDBMS
   void decode () const throw (adt::RuntimeException) {;}
};

}
}
}

#endif

