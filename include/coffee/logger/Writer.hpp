#ifndef coffee_logger_Writer_hpp
#define coffee_logger_Writer_hpp

#include <coffee/logger/Level.hpp>
#include <coffee/adt/RuntimeException.hpp>

namespace coffee {

namespace logger {

class Logger;

class Writer {
public:
   virtual ~Writer() {;}

protected:
   explicit Writer(const std::string& name) : m_name(name) {;}

   virtual bool wantsToProcess(const Level::_v level) const noexcept;

private:
   const std::string m_name;

   Writer(const Writer&);

   void initialize() throw(adt::RuntimeException) {
      do_initialize();
   }

   virtual void do_initialize() throw(adt::RuntimeException) = 0;
   virtual void apply(const Level::_v level, const std::string& line) noexcept = 0;

   friend class Logger;
};

}
}

#endif
