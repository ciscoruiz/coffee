#ifndef __coffee_balance_StrategyRoundRobin_hpp
#define __coffee_balance_StrategyRoundRobin_hpp

#include "Strategy.hpp"
#include "ResourceList.hpp"

namespace coffee {
namespace balance {

class GuardResourceList;

class StrategyRoundRobin : public Strategy {
public:
   explicit StrategyRoundRobin (std::shared_ptr<ResourceList>& resources) : Strategy("balance::RoundRobin", resources) {;}

   std::shared_ptr<Resource> apply() throw (ResourceUnavailableException);

private:
   class Position {
   public:
      Position() : valid(false) {;}

      operator bool() { return valid; }
      Position& operator=(ResourceList::resource_iterator other)  { iterator = other; valid = true; return *this; }
      ResourceList::resource_iterator value() { return iterator; }

   private:
      bool valid;
      ResourceList::resource_iterator iterator;
   };
   Position m_position;

   std::shared_ptr<Resource> apply(GuardResourceList& guard) throw (ResourceUnavailableException);
};

} /* namespace balance */
} /* namespace coffee */
#endif
