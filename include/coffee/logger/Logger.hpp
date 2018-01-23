// COFFEE - COmpany eFFEEctive Platform
//
//(c) Copyright 2018 Francisco Ruiz Rayo
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#ifndef coffee_logger_Logger_hpp
#define coffee_logger_Logger_hpp

#include <memory>
#include <vector>

#include <coffee/logger/Level.hpp>

#include <coffee/adt/RuntimeException.hpp>

namespace coffee {

namespace adt {
   class StreamString;
}

namespace logger {

class Writer;
class Formatter;

class Logger {
   typedef std::vector<std::shared_ptr<Writer> > Writers;
   typedef Writers::iterator writer_iterator;

public:
   static void initialize(std::shared_ptr<Writer> writer, std::shared_ptr<Formatter> formatter) throw(adt::RuntimeException);
   static void initialize(std::shared_ptr<Writer> writer) throw(adt::RuntimeException);
   static void add(std::shared_ptr<Writer> writer) throw (adt::RuntimeException);

   static void write(const Level::_v level, const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) noexcept;

   static void setLevel(const Level::_v level) noexcept { m_level = level; }
   static Level::_v getLevel() noexcept { return m_level; }

   static bool isActive(const Level::_v level) noexcept { return(level <= Level::Error) ? true:(level <= m_level); }
   static bool wantsToProcess(const Level::_v level) noexcept;

   static void critical(const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) noexcept { write(Level::Critical, streamString, function, file, line); }
   static void error(const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) noexcept { write(Level::Error, streamString, function, file, line); }
   static void warning(const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) noexcept { write(Level::Warning, streamString, function, file, line); }
   static void notice(const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) noexcept { write(Level::Notice, streamString, function, file, line); }
   static void info(const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) noexcept { write(Level::Information, streamString, function, file, line); }
   static void debug(const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) noexcept { write(Level::Debug, streamString, function, file, line); }

   static void write(const adt::Exception& ex) {
      error(ex.what(), ex.getMethod(), ex.getFile(), ex.getLine());
   }

private:
   static Level::_v m_level;
   static Writers m_writers;
   static std::shared_ptr<Formatter> m_formatter;

   Logger() = delete;
   Logger(const Logger&) = delete;
};

#ifdef COFFEE_LOG_LOCATION
#undef COFFEE_LOG_LOCATION
#endif
#define COFFEE_LOG_LOCATION __func__,__FILE__,__LINE__

#define LOG_CRITICAL(args)\
   do {\
   if(coffee::logger::Logger::wantsToProcess(coffee::logger::Level::Critical)) { \
      coffee::adt::StreamString msg; \
      coffee::logger::Logger::critical(msg << args, COFFEE_LOG_LOCATION); \
   } \
   } while(false);

#define LOG_ERROR(args)\
   do {\
   if(coffee::logger::Logger::wantsToProcess(coffee::logger::Level::Error)) { \
      coffee::adt::StreamString msg; \
      coffee::logger::Logger::error(msg << args, COFFEE_LOG_LOCATION); \
   } \
   } while(false);

#define LOG_WARN(args)\
   do {\
   if(coffee::logger::Logger::wantsToProcess(coffee::logger::Level::Warning)) { \
      coffee::adt::StreamString msg; \
      coffee::logger::Logger::warning(msg << args, COFFEE_LOG_LOCATION); \
   } \
   } while(false);

#define LOG_NOTICE(args)\
   do {\
   if(coffee::logger::Logger::wantsToProcess(coffee::logger::Level::Notice)) { \
      coffee::adt::StreamString msg; \
      coffee::logger::Logger::notice(msg << args, COFFEE_LOG_LOCATION); \
   } \
   } while(false);

#define LOG_INFO(args)\
   do {\
   if(coffee::logger::Logger::wantsToProcess(coffee::logger::Level::Information)) { \
      coffee::adt::StreamString msg; \
      coffee::logger::Logger::info(msg << args, COFFEE_LOG_LOCATION); \
   } \
   } while(false);

#define LOG_DEBUG(args)\
   do {\
   if(coffee::logger::Logger::wantsToProcess(coffee::logger::Level::Debug)) { \
      coffee::adt::StreamString msg; \
      coffee::logger::Logger::debug(msg << args, COFFEE_LOG_LOCATION); \
   } \
   } while(false);

#define LOG_LOCAL7(args)\
   do {\
   if(coffee::logger::Logger::wantsToProcess(coffee::logger::Level::Local7)) { \
      coffee::adt::StreamString msg; \
      coffee::logger::Logger::debug(msg << args, COFFEE_LOG_LOCATION); \
   } \
   } while(false);

}
}

#endif
