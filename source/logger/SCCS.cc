
#include <coffee/config/SCCSRepository.hpp>

#include <coffee/config/SCCS.hpp>
#include <coffee/adt/SCCS.hpp>

#include <coffee/logger/SCCS.hpp>

coffee_sccs_define_tag (logger, 1)

using namespace coffee;

void logger::SCCS::activate()
   noexcept
{
   adt::SCCS::activate ();

   config::SCCSRepository::getInstance().registerModule(coffee_sccs_use_tag(logger));
}

