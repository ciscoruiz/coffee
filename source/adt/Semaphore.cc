#include <coffee/adt/Semaphore.hpp>

using namespace coffee;

void adt::Semaphore::wait() noexcept
{
   std::unique_lock <std::mutex> guard (mutex);
   while (counter == 0) {
      condition.wait(guard);
   }
}

void adt::Semaphore::signal() noexcept
{
   std::unique_lock <std::mutex> guard (mutex);
   ++ counter;
   condition.notify_all();
}

