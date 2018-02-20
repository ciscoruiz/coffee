#ifndef __coffee_logger_TtyWriter_hpp
#define __coffee_logger_TtyWriter_hpp

#include <iostream>

#include <coffee/logger/Writer.hpp>

namespace coffee {
namespace logger {

class TtyWriter : public Writer {
public:
   TtyWriter () : Writer ("TtyWriter") {;}

private:
   void do_initialize () throw (adt::RuntimeException) {;}
   void apply (const Level::_v level, const std::string& line) noexcept {
      std::cout << line << std::endl;
   }
};

} /* namespace logger */
} /* namespace coffee */
#endif
