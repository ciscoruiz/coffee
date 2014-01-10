// WEPA - Write Excellent Professional Applications
//
// (c) Copyrigth 2013 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/wepa
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyrigth
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyrigth notice, this list of conditions and the following disclaimer
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
#include <functional>

#include <wepa/logger/Logger.hpp>
#include <wepa/logger/BacktraceWriter.hpp>

using namespace wepa;

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
