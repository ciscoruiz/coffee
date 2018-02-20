#ifndef __coffee_logger_Formatter_hpp
#define __coffee_logger_Formatter_hpp

#include <coffee/adt/StreamString.hpp>
#include <coffee/adt/NamedObject.hpp>

#include <coffee/logger/Level.hpp>

namespace coffee {

namespace logger {

class Logger;

class Formatter : public adt::NamedObject {
public:
   struct Elements {
      const Level::_v level;
      const adt::StreamString& input;
      const char* function;
      const char* file;
      const unsigned lineno;

      Elements (const Level::_v _level, const adt::StreamString& _input, const char* _function, const char* _file, const unsigned _lineno) :
         level (_level), input (_input), function (_function), file (_file), lineno (_lineno)
      {;}
   };

   virtual ~Formatter () {;}

protected:
   explicit Formatter (const std::string& name) : adt::NamedObject (name) {;}

   virtual std::string apply (const Elements& elements) noexcept = 0;

private:
   friend class Logger;

   Formatter (const Formatter&);
};

}
}

#endif
