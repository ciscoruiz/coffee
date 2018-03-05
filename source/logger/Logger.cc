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

#include <coffee/logger/Writer.hpp>
#include <coffee/logger/DefaultFormatter.hpp>
#include <coffee/logger/Level.hpp>
#include <coffee/logger/SCCS.hpp>
#include <coffee/logger/CircularTraceWriter.hpp>

using namespace coffee;

#ifdef _DEBUG
   logger::Level::_v logger::Logger::m_level = Level::Debug;
#else
   logger::Level::_v logger::Logger::m_level = Level::Warning;
#endif

std::shared_ptr<logger::Formatter> logger::Logger::m_formatter;
logger::Logger::Writers logger::Logger::m_writers;

//static
void logger::Logger::initialize(std::shared_ptr<Writer> writer, std::shared_ptr<Formatter> formatter)
   throw(adt::RuntimeException)
{
   SCCS::activate();

   m_writers.clear();
   m_writers.push_back(writer);
   m_formatter = formatter;

   writer->initialize();
}

//static
void logger::Logger::initialize(std::shared_ptr<Writer> writer)
   throw(adt::RuntimeException)
{
   initialize(writer, std::make_shared<DefaultFormatter>());
}

//static
void logger::Logger::add(std::shared_ptr<Writer> writer)
   throw (adt::RuntimeException)
{
   if (!m_formatter) {
      COFFEE_THROW_EXCEPTION("You should initialize the Logger before add a new writer");
   }

   writer->initialize();
   m_writers.push_back(writer);
}

//static
void logger::Logger::write(const Level::_v level, const adt::StreamString& input, const char* function, const char* file, const unsigned lineno)
   noexcept
{
   if (!wantsToProcess(level))
      return;

   const std::string string = m_formatter->apply(level, input, function, file, lineno);

   for (auto writer : m_writers) {
      if (writer->wantsToProcess(level)) {
         writer->apply(level, string);
      }
   }
}

//static
bool logger::Logger::wantsToProcess(const Level::_v level)
   noexcept
{
   if(m_writers.empty() || !m_formatter)
      return false;

   bool result = false;
   for (auto writer : m_writers) {
      if (writer->wantsToProcess(level)) {
         result = true;
         break;
      }
   }

   if (result == false)
      return false;

   return isActive(level) ? true: result;
}



