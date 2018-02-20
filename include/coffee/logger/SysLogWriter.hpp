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
