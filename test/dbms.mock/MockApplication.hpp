#ifndef __coffee_mock_MockApplication_hpp
#define __coffee_mock_MockApplication_hpp

#include <mutex>

#include <coffee/app/Application.hpp>

namespace coffee {
namespace mock {

class MockApplication : public app::Application {
public:
   std::mutex m_termination;

   explicit MockApplication(const char* title);

   void operator()();

private:
   void run() throw(adt::RuntimeException);
};

} /* namespace mock */
} /* namespace coffee */
#endif
