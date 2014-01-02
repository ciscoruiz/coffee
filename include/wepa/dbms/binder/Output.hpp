// WEPA - Write Excellent Professional Applications
#ifndef _wepa_dbms_binder_Output_h
#define _wepa_dbms_binder_Output_h

#include <wepa/dbms/binder/BinderIf.hpp>

namespace wepa {

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
class Output : public BinderIf {
public:
   adt::StreamString asString () const throw ();

   /**
      Graba el valor de la variable wepa::dbms::LongBlock asociada a esta Output. Cualquier
      modificacion que necesitemos aplicar sobre la columnna de tipo se debera hacer mediante los
      metodos ofrecidos por la clase wepa::dbms::LongBlock.

      \warning Este metodo solo puede ser usado para variables de tipo dbms::Data::Type::LongBlock y
      siempre y cuando hayamos abierto el BLOB con una sentencia SQL de seleccion.
   */
   void write () const throw (adt::RuntimeException, dbms::DatabaseException);

protected:
   Output (datatype::Abstract& value) : BinderIf (value) {;}

private:
   // este metodo no sera invocado nunca. A partir de un Output nunca hay que
   // convertir de C++ -> RDBMS
   void encode () const throw (adt::RuntimeException) {;}

   virtual void do_write (const datatype::LongBlock&) const throw (adt::RuntimeException, dbms::DatabaseException) = 0;
};

}
}
}

#endif

