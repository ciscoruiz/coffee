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
