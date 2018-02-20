#ifndef __coffee_mock_MockInput_hpp
#define __coffee_mock_MockInput_hpp

#include <coffee/dbms/binder/Input.hpp>

namespace coffee {
namespace mock {

class MockInput : public dbms::binder::Input {
public:
   explicit MockInput (std::shared_ptr<dbms::datatype::Abstract>& abstract) : dbms::binder::Input (abstract) {;}

private:
   void do_prepare (dbms::Statement& statement, const int pos) throw (adt::RuntimeException, dbms::DatabaseException) {;}
   void do_release (dbms::Statement& statement) noexcept {;}
   void do_encode (dbms::Statement& statement, const int pos) throw (adt::RuntimeException) {;}
};


} /* namespace mock */
} /* namespace coffee */

#endif
