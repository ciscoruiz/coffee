#include <coffee/balance/GuardResourceList.hpp>
#include <coffee/balance/ResourceList.hpp>

using namespace coffee;

balance::GuardResourceList::GuardResourceList(std::shared_ptr<ResourceList>& resourceList) :
      m_lock(new lock_guard(resourceList->m_mutex))
{}


