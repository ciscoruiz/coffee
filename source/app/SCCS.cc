
#include <coffee/config/SCCSRepository.hpp>

#include <coffee/config/SCCS.hpp>
#include <coffee/adt/SCCS.hpp>
#include <coffee/xml/SCCS.hpp>
#include <coffee/logger/SCCS.hpp>

#include <coffee/app/SCCS.hpp>

coffee_sccs_define_tag (app, 0)

using namespace coffee;

void app::SCCS::activate()
   noexcept
{
   config::SCCS::activate ();
   adt::SCCS::activate ();
   xml::SCCS::activate ();
   logger::SCCS::activate();

   config::SCCSRepository::getInstance().registerModule(coffee_sccs_use_tag(app));
}

