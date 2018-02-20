#ifndef __coffee_logger_SysLogWriter_hpp
#define __coffee_logger_SysLogWriter_hpp

#include <coffee/logger/Writer.hpp>

namespace coffee {

namespace logger {

class SysLogWriter : public Writer {
public:
	/**
	 * This class will avoid to use macros defined in syslog.h which
	 * will cause conflicts with macros defined into coffee::logger::Logger
	 */
   class OptionBuilder {
	   int flags;

	   int build() const { return flags; }

   public:
	   OptionBuilder() { flags = 0; }

	   OptionBuilder& showPid() noexcept;
	   OptionBuilder& consoleWhenError() noexcept;
	   OptionBuilder& delayOpen() noexcept;
	   OptionBuilder& noDelayOpen() noexcept;
	   OptionBuilder& logToStdErr() noexcept;

	   friend class SysLogWriter;
   };

   static const int MinimalSize = 256;
   static const int NullStream;
   static const int CheckSizePeriod;

   SysLogWriter (const std::string& ident, const OptionBuilder& builder);
   virtual ~SysLogWriter ();

private:
   const std::string m_ident;
   const int m_options;

   void do_initialize () throw (adt::RuntimeException);
   void apply (const Level::_v level, const std::string& line) noexcept;
};

}
}

#endif
