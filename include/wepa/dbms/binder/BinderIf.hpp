// WEPA - Write Excellent Professional Applications
#ifndef _wepm_dbms_binder_Abstract_h
#define _wepm_dbms_binder_Abstract_h

#include <wepa/adt/RuntimeException.hpp>

#include <wepa/dbms/DatabaseException.hpp>

namespace wepa {

namespace dbms {

namespace datatype {
class Abstract;
}

class Connection;
class Statement;

namespace binder {

class BinderIf {
public:
   virtual ~BinderIf () {;}

   datatype::Abstract& getData () throw () { return m_data; }
   const datatype::Abstract& getData () const throw () { return m_data; }

   virtual void prepare (Statement* statement, Connection* connection, const int pos) throw (adt::RuntimeException, DatabaseException) = 0;
   virtual void release (Statement* statement) throw () = 0;
   virtual void encode () const throw (adt::RuntimeException) = 0;
   virtual void decode () const throw (adt::RuntimeException) = 0;
   virtual adt::StreamString asString () const throw () = 0;

protected:
   BinderIf (dbms::datatype::Abstract& data) : m_data (data) {;}

private:
   datatype::Abstract& m_data;

   friend class Statement;
};

}
}
}

#endif

