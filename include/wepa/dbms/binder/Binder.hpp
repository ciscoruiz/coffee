// WEPA - Write Excellent Professional Applications
#ifndef _wepm_dbms_binder_Binder_h
#define _wepm_dbms_binder_Binder_h

#include <functional>
#include <memory>

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

class Binder {
public:
   virtual ~Binder() {;}

   std::shared_ptr<datatype::Abstract>& getData() noexcept { return std::ref(m_data); }
   const std::shared_ptr<datatype::Abstract>& getData() const noexcept { return std::ref(m_data); }

protected:
   Binder(std::shared_ptr<datatype::Abstract>& data) : m_data(data) {;}

   virtual adt::StreamString asString() const noexcept = 0;

private:
   std::shared_ptr<datatype::Abstract> m_data;

   virtual void do_prepare(Statement& statement, const int pos) throw(adt::RuntimeException, DatabaseException) = 0;
   virtual void do_release(Statement& statement) noexcept = 0;
   virtual void do_encode() throw(adt::RuntimeException) = 0;
   virtual void do_decode() throw(adt::RuntimeException) = 0;

   friend class wepa::dbms::Statement;
};

}
}
}

#endif

