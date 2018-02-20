
#include <coffee/config/SCCS.hpp>
#include <coffee/config/SCCSRepository.hpp>

coffee_sccs_define_tag (config, 0)

using namespace coffee;

void config::SCCS::activate()
   noexcept
{
   SCCSRepository::getInstance().registerModule(coffee_sccs_use_tag(config));
}

