#include <coffee/logger/DefaultFormatter.hpp>

#include <coffee/adt/StreamString.hpp>
#include <coffee/adt/Second.hpp>

using namespace coffee;
using namespace coffee::logger;

std::string DefaultFormatter::apply (const Elements& elements)
   noexcept
{
   auto second = adt::Second::getLocalTime ();

   adt::StreamString output;

   output << "[" << second.asDateTime() << "] ";
   output << Level::enumName(elements.level) << " | ";
   output << elements.function << " [" << elements.file << "(" << elements.lineno << ")]: ";
   output << elements.input;

   return output;
}
