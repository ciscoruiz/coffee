#ifndef __coffee_dbms_internal_DummyApplication_hpp
#define __coffee_dbms_internal_DummyApplication_hpp

#include <functional>

#include <coffee/app/Application.hpp>

namespace coffee {

namespace app {
class Engine;
}

namespace dbms {
namespace internal {

class DummyApplication : public app::Application {
public:
   DummyApplication () : app::Application ("dummy", "dummy", "1.0") {;}

   static app::Application& getInstance () noexcept { return std::ref (st_this); }

private:
   static DummyApplication st_this;

   void run () throw (adt::RuntimeException) {;}
};


} /* namespace internal */
} /* namespace dbms */
} /* namespace coffee */
#endif
