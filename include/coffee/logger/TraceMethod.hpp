#ifndef __wepm_logger_TraceMethod_hpp
#define __wepm_logger_TraceMethod_hpp

#include <coffee/logger/Level.hpp>
#include <coffee/logger/Logger.hpp>

namespace coffee {

namespace logger {

class TraceMethod  {
public:
   TraceMethod (const char* methodName, const char* fromFile, const int fromLine) :
      m_level (Level::Debug),
      m_methodName (methodName),
      m_fromFile (fromFile),
      m_ok (false)
   {
      if ((m_ok = Logger::wantsToProcess (m_level)) == true)
         Logger::write (m_level, "+++ begin +++", methodName, fromFile, fromLine);
   }

   TraceMethod (const Level::_v level, const char* methodName, const char* fromFile, const int fromLine) :
      m_level (level),
      m_methodName (methodName),
      m_fromFile (fromFile),
      m_ok (false)
   {
      if ((m_ok = Logger::wantsToProcess (m_level)) == true)
         Logger::write (m_level, "+++ begin +++", methodName, fromFile, fromLine);
   }

   /**
      Destructor.
   */
   ~TraceMethod () {
      if (m_ok == true && Logger::wantsToProcess(m_level) == true) {
         Logger::write (m_level, "+++ end +++", m_methodName, m_fromFile, 0);
      }
   }

private:
   const Level::_v m_level;
   const char* m_methodName;
   const char* m_fromFile;
   bool m_ok;
};

}
}

#define LOG_THIS_METHOD() coffee::logger::TraceMethod __traceMethod__ (COFFEE_LOG_LOCATION)

#endif
