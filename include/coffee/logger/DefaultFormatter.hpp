#ifndef __coffee_logger_DefaultFormatter_hpp
#define __coffee_logger_DefaultFormatter_hpp

#include <coffee/logger/Formatter.hpp>

namespace coffee {
namespace logger {

class DefaultFormatter : public Formatter {
public:
   DefaultFormatter () : Formatter ("DefaultFormatter") {;}

private:
   std::string apply (const Elements& elements) noexcept;

};

}
}
#endif
