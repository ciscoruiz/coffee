#ifndef __coffee_balance_StrategyIndexed_hpp
#define __coffee_balance_StrategyIndexed_hpp

#include "Strategy.hpp"

namespace coffee {
namespace balance {

class GuardResourceList;

class StrategyIndexed : public Strategy {
public:
   explicit StrategyIndexed (std::shared_ptr<ResourceList>& resources) :
      balance::Strategy("balance::Indexed", resources),
      m_key(0)
   {
   }

   std::shared_ptr<Resource> apply(const int key) throw (ResourceUnavailableException);

private:
   int m_key;

   std::shared_ptr<Resource> apply(GuardResourceList& guard) throw (ResourceUnavailableException);
};

} /* namespace balance */
} /* namespace coffee */
#endif
