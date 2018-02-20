#ifndef __coffee_logger_BacktraceWriter_hpp
#define __coffee_logger_BacktraceWriter_hpp

#include <list>

#include <coffee/logger/CircularTraceWriter.hpp>

namespace coffee {
namespace logger {

class BacktraceWriter : public CircularTraceWriter {
public:
   static const int MaxBacktrackingLength;

   BacktraceWriter (const std::string& path, const size_t maxSize, const int backtrackingLength);

   void setLowestLeveL (const Level::_v lowestLeveL) noexcept { if (lowestLeveL > Level::Error) m_lowestLevel = lowestLeveL; }

private:
   typedef std::pair <Level::_v, std::string> Line;
   typedef std::list <Line> Lines;

   const int m_backtrackingLength;
   Lines m_lines;
   int m_lineCounter;
   Level::_v m_lowestLevel;

   void apply (const Level::_v level, const std::string& line) noexcept;
   bool wantsToProcess (const Level::_v level) const noexcept { return level <= m_lowestLevel; }
   void backtrace () noexcept;
};

} /* namespace logger */
} /* namespace coffee */

#endif
