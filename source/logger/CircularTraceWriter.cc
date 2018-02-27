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

#include <iostream>

#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include <algorithm>

#include <coffee/logger/CircularTraceWriter.hpp>

using namespace coffee;

//static
const int logger::CircularTraceWriter::NullStream = -1;

logger::CircularTraceWriter::CircularTraceWriter(const std::string& path, const size_t maxKbSize) :
  logger::Writer ("CircularTraceWriter"),
  m_path (path),
  m_stream (NullStream),
  m_lineno (0)
{
   m_maxKbSize = std::max (size_t (MinimalKbSize), maxKbSize) * 1024;
}

void logger::CircularTraceWriter::do_initialize() throw (adt::RuntimeException)
{
   openStream ();

   if (oversizedStream () == true) {
      closeStream ();
      renameFile ();
      openStream ();
   }
}

void logger::CircularTraceWriter::apply(const Level::_v level, const std::string& line)
   noexcept
{
   if (m_stream == NullStream) {
      if (level <= Level::Error) {
         std::cerr << line << std::endl;
         ++ m_lineno;
      }
      return;
   }

   try {
      write (m_stream, line.data (), line.length());
      write (m_stream, "\n", 1);

      if ((++ m_lineno % CheckSizePeriod) != 0)
         return;

      if (oversizedStream () == true) {
         closeStream ();
         renameFile ();
         openStream ();
      }
   }
   catch (adt::RuntimeException& ex) {
      std::cerr << ex.what() << std::endl;
   }
}

// When there is some kind of error over the stream, it will only trace error's
bool logger::CircularTraceWriter::wantsToProcess (const logger::Level::_v level) const
   noexcept
{
   return (m_stream != NullStream) ? logger::Writer::wantsToProcess(level): level <= Level::Error;
}

void coffee::logger::CircularTraceWriter::openStream()
   throw (adt::RuntimeException)
{
   int stream = open (m_path.c_str (), O_RDWR | O_CREAT | O_APPEND, S_IRUSR |S_IWUSR | S_IRGRP| S_IROTH);

   if (stream == -1)
      COFFEE_THROW_EXCEPTION("Can not open file: " << m_path << ". Error: " << strerror(errno));

   m_stream = stream;

   fcntl (stream, F_SETFL, fcntl (stream, F_GETFL) | O_NONBLOCK);
}

bool coffee::logger::CircularTraceWriter::oversizedStream()
   throw (adt::RuntimeException)
{
   if (m_stream == NullStream)
      return false;

   struct stat data;
   int r;

   if (fstat (m_stream, &data) == -1)
      COFFEE_THROW_EXCEPTION("Can not get file length: " << m_path << ". Error: " << strerror(errno));

   return data.st_size > m_maxKbSize;
}

void logger::CircularTraceWriter::closeStream()
   noexcept
{
   if (m_stream != NullStream)
      ::close (m_stream);

   m_stream = NullStream;
}

void logger::CircularTraceWriter::renameFile()
   throw (adt::RuntimeException)
{
   std::string file_old (m_path);
   file_old.append (".old");

   if (rename (m_path.c_str (), file_old.c_str ()) != 0) {
      int xerrno = errno;
      truncate (m_path.c_str (), 0);
      COFFEE_THROW_EXCEPTION("Could not rename: " << m_path << ". Error: " << strerror(xerrno));
   }
}
