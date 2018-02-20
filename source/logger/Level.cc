#include <coffee/logger/Level.hpp>

using namespace coffee;

//static
const char* logger::Level::enumName(const Level::_v level)
   noexcept
{
   static const char* names[] = {
     "Emergency", "Alert", "Critical", "Error", "Warning", "Notice", "Information", "Debug",
     "Local0", "Local1", "Local2", "Local3", "Local4", "Local5", "Local6", "Local7"
   };

   return (level == Level::None) ? "None": names[level];
}
