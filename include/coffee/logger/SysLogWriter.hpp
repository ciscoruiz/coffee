// COFFEE - COmpany eFFEEctive Platform
//
// (c) Copyright 2018 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/coffee
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
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
