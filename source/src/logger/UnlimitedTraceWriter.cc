// MIT License
// 
// Copyright(c) 2018 Francisco Ruiz(francisco.ruiz.rayo@gmail.com)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
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

#include <iostream>

#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include <algorithm>

#include <coffee/logger/UnlimitedTraceWriter.hpp>

using namespace coffee;

//static
const int logger::UnlimitedTraceWriter::NullStream = -1;

logger::UnlimitedTraceWriter::UnlimitedTraceWriter(const std::string& path) :
  logger::Writer("UnlimitedTraceWriter"),
  m_path(path),
  m_stream(NullStream)
{
}

void logger::UnlimitedTraceWriter::initialize() throw(basis::RuntimeException)
{
   openStream();
}

void logger::UnlimitedTraceWriter::apply(const Level::_v level, const std::string& line)
   noexcept
{
   if (m_stream == NullStream) {
      if (level <= Level::Error) {
         std::cerr << line << std::endl;
      }
      return;
   }

   try {
      write(m_stream, line.data(), line.length());
      write(m_stream, "\n", 1);
   }
   catch(basis::RuntimeException& ex) {
      std::cerr << ex.what() << std::endl;
   }
}

// When there is some kind of error over the stream, it will only trace error's
bool logger::UnlimitedTraceWriter::wantsToProcess(const logger::Level::_v level) const
   noexcept
{
   return(m_stream != NullStream) ? logger::Writer::wantsToProcess(level): level <= Level::Error;
}

void coffee::logger::UnlimitedTraceWriter::openStream()
   throw(basis::RuntimeException)
{
   int stream = open(m_path.c_str(), O_RDWR | O_CREAT | O_APPEND, S_IRUSR |S_IWUSR | S_IRGRP| S_IROTH);

   if (stream == -1)
      COFFEE_THROW_EXCEPTION("Can not open file: " << m_path << ". Error: " << strerror(errno));

   m_stream = stream;

   fcntl(stream, F_SETFL, fcntl(stream, F_GETFL) | O_NONBLOCK);
}

void logger::UnlimitedTraceWriter::closeStream()
   noexcept
{
   if (m_stream != NullStream)
      ::close(m_stream);

   m_stream = NullStream;
}


