// WEPA - Write Excellent Professional Applications
#ifndef _wepm_dbms_binder_Abstract_h
#define _wepm_dbms_binder_Abstract_h

#include <functional>

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

   datatype::Abstract& getData () throw () { return std::ref (m_data); }
   const datatype::Abstract& getData () const throw () { return std::ref (m_data); }

protected:
   BinderIf (dbms::datatype::Abstract& data) : m_data (data) {;}

   virtual adt::StreamString asString () const throw () = 0;

private:
   datatype::Abstract& m_data;

   virtual void do_prepare (Statement* statement, Connection* connection, const int pos) throw (adt::RuntimeException, DatabaseException) = 0;
   virtual void do_release (Statement* statement) throw () = 0;
   virtual void do_encode () throw (adt::RuntimeException) = 0;
   virtual void do_decode () throw (adt::RuntimeException) = 0;

   friend class wepa::dbms::Statement;
};

}
}
}

#endif

