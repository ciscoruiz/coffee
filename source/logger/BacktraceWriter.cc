
#include <coffee/logger/Logger.hpp>
#include <coffee/logger/BacktraceWriter.hpp>

using namespace coffee;

//static
const int logger::BacktraceWriter::MaxBacktrackingLength = 128;

logger::BacktraceWriter::BacktraceWriter (const std::string& path, const size_t maxSize, const int backtrackingLength) :
  CircularTraceWriter(path, maxSize),
  m_backtrackingLength (std::min (backtrackingLength, MaxBacktrackingLength)),
  m_lineCounter (0),
  m_lowestLevel (Level::Debug)
{
}

void logger::BacktraceWriter::apply (const Level::_v level, const std::string& line)
   noexcept
{
   if (level <= Level::Error) {
      backtrace ();
      CircularTraceWriter::apply(level, line);
   }
   else if (level > Level::Error && level <= m_lowestLevel) {
      Line storedLine (level, line);

      if (m_lineCounter == m_backtrackingLength) {
         m_lines.pop_front();
         m_lines.push_back(storedLine);
      }
      else {
         m_lines.push_back(storedLine);
         m_lineCounter ++;
      }
   }
}

void logger::BacktraceWriter::backtrace()
   noexcept
{
   Level::_v originalLevel = Logger::getLevel();

   Logger::setLevel (m_lowestLevel);

   for (auto ii = m_lines.begin (), maxii = m_lines.end (); ii != maxii; ++ ii) {
      auto storedLine = *ii;
      CircularTraceWriter::apply(storedLine.first, storedLine.second);
   }

   m_lines.clear();
   m_lineCounter = 0;

   Logger::setLevel(originalLevel);
}
