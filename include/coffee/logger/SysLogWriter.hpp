// MIT License
// 
// Copyright (c) 2018 Francisco Ruiz (francisco.ruiz.rayo@gmail.com)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#ifndef __coffee_logger_SysLogWriter_hpp
#define __coffee_logger_SysLogWriter_hpp

#include <coffee/logger/Writer.hpp>

namespace coffee {

namespace logger {

/**
 * This logger will use the syslog subsystem to write the selected traces.
 *
 * \include test/logger/SysLogWriter_test.cc
 */
class SysLogWriter : public Writer {
public:
	/**
	 * This class will avoid to use macros defined in syslog.h which
	 * will cause conflicts with macros defined into coffee::logger::Logger
	 */
   class OptionBuilder {
	   int flags;

   public:
	   OptionBuilder() { flags = 0; }

	   /**
	    * Activate the LOG_PID option.
	    */
	   OptionBuilder& showPid() noexcept;

	   /**
	    * Activate the LOG_CONS option.
	    */
	   OptionBuilder& consoleWhenError() noexcept;

      /**
       * Activate the LOG_ODELAY option.
       */
	   OptionBuilder& delayOpen() noexcept;

	   /**
	    * Activate the LOG_NDELAY option.
	    */
	   OptionBuilder& noDelayOpen() noexcept;

	   /**
	    * Activate the LOG_PERROR option.
	    */
	   OptionBuilder& logToStdErr() noexcept;

	   /**
	    * \return the Flags used by the SysLogWriter.
	    */
      int build() const { return flags; }

	   friend class SysLogWriter;
   };

   /**
    * Constructor.
    * \param ident The string pointed to by ident is prepended to every message,
    *  and is typically set to the program name is used.
    * \param builder Instance which contains the specific options.
    */
   SysLogWriter (const std::string& ident, const OptionBuilder& builder);

   /**
    * Destructor.
    */
   ~SysLogWriter ();

private:
   const std::string m_ident;
   const int m_options;

   void initialize () throw (basis::RuntimeException);
   void apply (const Level::_v level, const std::string& line) noexcept;
};

}
}

#endif
