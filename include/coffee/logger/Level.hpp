#ifndef coffee_logger_Level_hpp
#define coffee_logger_Level_hpp

#include <coffee/adt/RuntimeException.hpp>

namespace coffee {

namespace logger {

struct Level {
   enum _v { None = -1,  Emergency, Alert, Critical, Error, Warning, Notice, Information, Debug,
      Local0, Local1, Local2, Local3, Local4, Local5, Local6, Local7
   };

   static const char* enumName(const Level::_v level) noexcept;
};

}
}

#endif
