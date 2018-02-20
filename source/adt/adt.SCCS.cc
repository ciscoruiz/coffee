
#include <coffee/config/SCCSRepository.hpp>

#include <coffee/config/SCCS.hpp>
#include <coffee/adt/SCCS.hpp>

coffee_sccs_define_tag (adt, 1)

using namespace coffee;

void adt::SCCS::activate()
   noexcept
{
   config::SCCS::activate ();

   config::SCCSRepository::getInstance().registerModule(coffee_sccs_use_tag(adt));
}

