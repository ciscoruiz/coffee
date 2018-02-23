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

/**
 * Facade for the logger system. Not matter the Writer nor Formatter you will always used this interface.
 *
 * \include test/logger/filtering_test.cc
 */
class Logger {
   typedef std::vector<std::shared_ptr<Writer> > Writers;
   typedef Writers::iterator writer_iterator;

public:
   /**
    * Initialize the logger with the writer and the formatter received as parameter.
    * \warning Before calling that method the calling of method #critical, #error, ..., #debug will have no effect.
    */
   static void initialize(std::shared_ptr<Writer> writer, std::shared_ptr<Formatter> formatter) throw(adt::RuntimeException);

   /**
   * Initialize the logger with the writer received as parameter and the default formatter (DefaultFormatter).
   * \warning Before calling that method the calling of method #critical, @error, ..., #debug will have no effect.
   */
   static void initialize(std::shared_ptr<Writer> writer) throw(adt::RuntimeException);

   /**
    * You can attach a undefined numbers of writers to the this Logger, and all of them will received
    * the string composed by the formatter.
    */
   static void add(std::shared_ptr<Writer> writer) throw (adt::RuntimeException);

   /**
    * Write the trace received.
    * \li Call the formatter to get the final string to trace.
    * \li Call the writer to store that string.
    *
    * \param level Active Level with the trace was generated
    * \param streamString Contains the message to be used
    * \param function method name where the trace was created
    * \param file The file where the trace was created
    * \param line The line number where the trace was created.
    */
   static void write(const Level::_v level, const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) noexcept;

   /**
    * Change the current level. Only traces with a level smaller that this level will be traced.
    */
   static void setLevel(const Level::_v level) noexcept { m_level = level; }

   /**
    * \return Get current level.
    */
   static Level::_v getLevel() noexcept { return m_level; }

   /**
    * \return \b true if a trace using this the received level would be traced (taking account of the current level) or \b false otherwise.
    */
   static bool isActive(const Level::_v level) noexcept { return(level <= Level::Error) ? true:(level <= m_level); }

   /**
    * \return \b true if some associated writer would process the received trace or \b false otherwise.
    * Think about BacktraceWriter which wants to process all levels but it will not write anything till the
    * moment a error trace is detected.
    * \warning It is possible that a writer process some trace but it will not "write" anything by now.
    */
   static bool wantsToProcess(const Level::_v level) noexcept;

   /**
    * Send a trace with level Level::Critical to attached writers.
    * \warning Use this method through
    * \code
    *     LOG_CRITICAL(var1 << " some text " << var2 << " more text " << ... << varn);
    * \endcode
    */
   static void critical(const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) noexcept { write(Level::Critical, streamString, function, file, line); }

   /**
    * Send a trace with level Level::Error to attached writers.
    * \warning Use this method through
    * \code
    *     LOG_ERROR(var1 << " some text " << var2 << " more text " << ... << varn);
    * \endcode
    */
   static void error(const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) noexcept { write(Level::Error, streamString, function, file, line); }

   /**
    * Send a trace with level Level::Warning to attached writers.
    * \warning Use this method through
    * \code
    *     LOG_WARN(var1 << " some text " << var2 << " more text " << ... << varn);
    * \endcode
    */
   static void warning(const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) noexcept { write(Level::Warning, streamString, function, file, line); }

   /**
    * end a trace with level Level::Notice to attached writers.
    * \warning Use this method through
    * \code
    *
    *    LOG_NOTICE(var1 << " some text " << var2 << " more text " << ... << varn);
    * \endcode
    */
   static void notice(const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) noexcept { write(Level::Notice, streamString, function, file, line); }

   /**
    * end a trace with level Level::Information to attached writers.
    * \warning Use this method through
    * \code
    *
    *    LOG_INFO(var1 << " some text " << var2 << " more text " << ... << varn);
    * \endcode
    */
   static void info(const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) noexcept { write(Level::Information, streamString, function, file, line); }

   /**
    * end a trace with level Level::Debug to attached writers.
    * \warning Use this method through
    * \code
    *
    *    LOG_DEBUG(var1 << " some text " << var2 << " more text " << ... << varn);
    * \endcode
    */
   static void debug(const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) noexcept { write(Level::Debug, streamString, function, file, line); }

   /**
    * Send the trace of error with the information of the exception received.
    */
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
