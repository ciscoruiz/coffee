#ifndef __coffee_balance_GuardResourceList_hpp
#define __coffee_balance_GuardResourceList_hpp

#include <memory>
#include <mutex>

namespace coffee {

namespace balance {

class ResourceList;

class GuardResourceList {
   typedef std::lock_guard<std::mutex> lock_guard;

public:
   explicit GuardResourceList(std::shared_ptr<ResourceList>& resourceList);
   explicit GuardResourceList(std::mutex& mutex) : m_lock(new lock_guard(mutex)) {}
   ~GuardResourceList() { m_lock.reset(); }

private:
   std::unique_ptr<lock_guard> m_lock;

   // Need it for Fake-Guard
   explicit GuardResourceList(const ResourceList& resourceList) {}

   friend class ResourceList;
};

}
}

#endif
