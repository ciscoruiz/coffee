// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2013 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/wepa
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
#ifndef wepa_logger_Logger_hpp
#define wepa_logger_Logger_hpp

#include <memory>

#include <wepa/logger/Level.hpp>

#include <wepa/adt/RuntimeException.hpp>

namespace wepa {

namespace adt {
   class StreamString;
}

namespace logger {

class Writer;
class Formatter;

class Logger  {
public:
   typedef std::shared_ptr <Writer> WriterPtr;
   typedef std::shared_ptr <Formatter> FormatterPtr;

   static void initialize (Writer* writer, Formatter* formatter) throw (adt::RuntimeException);
   static void initialize (Writer* writer) throw (adt::RuntimeException);

   static void write (const Level::_v level, const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) throw ();

   static void setLevel (const Level::_v level) throw () { m_level = level; }

   static bool isActive (const Level::_v level) {
      return (level <= Level::Error) ? true: (level <= m_level);
   }

   static void error (const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) throw () { write (Level::Error, streamString, function, file, line); }
   static void warning (const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) throw () { write (Level::Warning, streamString, function, file, line); }
   static void notice (const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) throw () { write (Level::Notice, streamString, function, file, line); }
   static void info (const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) throw () { write (Level::Information, streamString, function, file, line); }
   static void debug (const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) throw () { write (Level::Debug, streamString, function, file, line); }

   static void write (const adt::RuntimeException& ex) {
      error (ex.what (), ex.getMethod(), ex.getFile(), ex.getLine());
   }
private:
   static Level::_v m_level;
   static WriterPtr m_writer;
   static FormatterPtr m_formatter;

   Logger ();
   Logger (const Logger&);
};

#define LOG_ERROR(args)\
   do {\
   if (wepa::logger::Logger::isActive (wepa::logger::Level::Error)) { \
      wepa::adt::StreamString msg; \
      wepa::logger::Logger::error (msg << args, WEPA_FILE_LOCATION); \
   } \
   } while (false);

#define LOG_WARN(args)\
   do {\
   if (wepa::logger::Logger::isActive (wepa::logger::Level::Warning)) { \
      wepa::adt::StreamString msg; \
      wepa::logger::Logger::warning (msg << args, WEPA_FILE_LOCATION); \
   } \
   } while (false);

#define LOG_NOTICE(args)\
   do {\
   if (wepa::logger::Logger::isActive (wepa::logger::Level::Notice)) { \
      wepa::adt::StreamString msg; \
      wepa::logger::Logger::notice(msg << args, WEPA_FILE_LOCATION); \
   } \
   } while (false);

#define LOG_INFO(args)\
   do {\
   if (wepa::logger::Logger::isActive (wepa::logger::Level::Information)) { \
      wepa::adt::StreamString msg; \
      wepa::logger::Logger::info (msg << args, WEPA_FILE_LOCATION); \
   } \
   } while (false);

#define LOG_DEBUG(args)\
   do {\
   if (wepa::logger::Logger::isActive (wepa::logger::Level::Debug)) { \
      wepa::adt::StreamString msg; \
      wepa::logger::Logger::debug (msg << args, WEPA_FILE_LOCATION); \
   } \
   } while (false);

}
}

#endif
