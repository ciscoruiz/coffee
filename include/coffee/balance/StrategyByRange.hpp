#ifndef __coffee_balance_StrategyByRange_hpp
#define __coffee_balance_StrategyByRange_hpp

#include <vector>
#include <memory>
#include <tuple>

#include "Strategy.hpp"

namespace coffee {
namespace balance {

class GuardResourceList;

class StrategyByRange : public Strategy {
   typedef std::tuple<int, int, std::shared_ptr<Strategy> > Range;
   typedef std::vector<Range> Ranges;
   typedef Ranges::iterator range_iterator;

public:
   /**
    * Constructor
    */
   StrategyByRange ();

   /**
    * Destructor
    */
   ~StrategyByRange () { m_ranges.clear (); }

   /**
    * \warning Once you call this method you can not append more resources to this \em balanceIf
    * @param bottom Minimal value for this range
    * @param top Maximal value for this range
    * @param balance Load balancing algorithm used under this range.
    */
   void addRange (const int bottom, const int top, std::shared_ptr<Strategy>& strategy) throw (adt::RuntimeException);

   std::shared_ptr<Resource> apply(const int key) throw (ResourceUnavailableException);

private:
   std::shared_ptr<ResourceList> m_unusedList;
   Ranges m_ranges;
   int m_key;

   range_iterator findRange (GuardResourceList&, const int key) noexcept;
   range_iterator range_begin() noexcept { return m_ranges.begin (); }
   range_iterator range_end() noexcept { return m_ranges.end (); }
   static Range& range(range_iterator ii) noexcept { return *ii; }

   std::shared_ptr<Resource> apply(GuardResourceList& guard) throw (ResourceUnavailableException);
};

} /* namespace balance */
} /* namespace coffee */
#endif
