#ifndef _coffee_adt_Semaphore_h
#define _coffee_adt_Semaphore_h

#include <mutex>
#include <condition_variable>

namespace coffee {
namespace adt {

class Semaphore {
public:
   explicit Semaphore(const int _counter) : counter(_counter) {;}
   void wait() noexcept;
   void signal() noexcept;

private:
   std::mutex mutex;
   std::condition_variable condition;
   int counter;
};

} /* namespace adt */
} /* namespace coffee */

#endif /* INCLUDE_COFFEE_ADT_SEMAPHORE_HPP_ */
