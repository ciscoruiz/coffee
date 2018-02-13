// COFFEE - COmpany eFFEEctive Platform
#ifndef _wepm_dbms_binder_Binder_h
#define _wepm_dbms_binder_Binder_h

#include <functional>
#include <memory>

#include <coffee/adt/RuntimeException.hpp>

#include <coffee/dbms/DatabaseException.hpp>

namespace coffee {

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
   explicit Binder(std::shared_ptr<datatype::Abstract>& data) : m_data(data) {;}

   virtual adt::StreamString asString() const noexcept = 0;

private:
   std::shared_ptr<datatype::Abstract> m_data;

   virtual void do_prepare(Statement& statement, const int pos) throw(adt::RuntimeException, DatabaseException) = 0;

   friend class coffee::dbms::Statement;
};

}
}
}

#endif

