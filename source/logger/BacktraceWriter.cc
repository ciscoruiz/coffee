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


#include <coffee/logger/Logger.hpp>
#include <coffee/logger/BacktraceWriter.hpp>

using namespace coffee;

//static
const int logger::BacktraceWriter::MaxBacktrackingLength = 4096;

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
