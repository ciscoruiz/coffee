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
#include <coffee/logger/Logger.hpp>

#include <coffee/logger/Writer.hpp>
#include <coffee/logger/DefaultFormatter.hpp>
#include <coffee/logger/Level.hpp>
#include <coffee/logger/SCCS.hpp>

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

   Formatter::Elements elements(level, input, function, file, lineno);
   const std::string string = m_formatter->apply(elements);

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



