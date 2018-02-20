#ifndef __coffee_mock_MockOutput_hpp
#define __coffee_mock_MockOutput_hpp

#include <coffee/dbms/binder/Output.hpp>

namespace coffee {
namespace mock {

class MockOutput : public dbms::binder::Output {
public:
   explicit MockOutput(std::shared_ptr<dbms::datatype::Abstract>& abstract) : dbms::binder::Output(abstract) {;}

private:
   void do_prepare(dbms::Statement& statement, const int pos) throw(adt::RuntimeException, dbms::DatabaseException) {;}
   void do_release(dbms::Statement& statement) noexcept {;}
   void do_decode(dbms::Statement& statement, const int pos) throw(adt::RuntimeException) {;}
   void do_write(const std::shared_ptr<dbms::datatype::LongBlock>&) throw(adt::RuntimeException, dbms::DatabaseException) {;}
};

} /* namespace mock */
} /* namespace coffee */
#endif
