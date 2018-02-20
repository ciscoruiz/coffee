#include <coffee/logger/Writer.hpp>
#include <coffee/logger/Logger.hpp>

using namespace coffee;

// virtual
bool logger::Writer::wantsToProcess (const logger::Level::_v level) const
   noexcept
{
   return Logger::isActive(level);
}
