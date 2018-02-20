
#include <coffee/config/SCCSRepository.hpp>

#include <coffee/config/SCCS.hpp>
#include <coffee/adt/SCCS.hpp>

#include <coffee/xml/SCCS.hpp>

coffee_sccs_define_tag (xml, 1)

using namespace coffee;

void xml::SCCS::activate()
   noexcept
{
   adt::SCCS::activate ();

   config::SCCSRepository::getInstance().registerModule(coffee_sccs_use_tag(xml));
}

