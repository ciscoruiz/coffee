
#include <coffee/config/SCCSRepository.hpp>

#include <coffee/config/SCCS.hpp>
#include <coffee/adt/SCCS.hpp>
#include <coffee/xml/SCCS.hpp>
#include <coffee/logger/SCCS.hpp>

#include <coffee/balance/SCCS.hpp>

coffee_sccs_define_tag (balance, 0)

using namespace coffee;

void balance::SCCS::activate()
   noexcept
{
   config::SCCS::activate ();
   adt::SCCS::activate ();
   xml::SCCS::activate ();
   logger::SCCS::activate();

   config::SCCSRepository::getInstance().registerModule(coffee_sccs_use_tag(balance));
}

