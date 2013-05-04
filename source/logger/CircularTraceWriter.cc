// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2013 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/wepa
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#include <iostream>

#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <algorithm>

#include <wepa/logger/CircularTraceWriter.hpp>

using namespace wepa;

//static
const int logger::CircularTraceWriter::NullStream = -1;

//static
const int logger::CircularTraceWriter::CheckSizePeriod = 128;

logger::CircularTraceWriter::CircularTraceWriter(const std::string& path, const size_t maxSize) :
  logger::Writer ("CircularTraceWriter"),
  m_path (path),
  m_stream (NullStream),
  m_lineno (0)
{
   m_maxSize = std::max (size_t (MinimalSize), maxSize) * 1024;
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
   throw ()
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

      if ((++ m_lineno % CheckSizePeriod) == 0) {
         if (oversizedStream () == true) {
            closeStream ();
            renameFile ();
            openStream ();
            logger::Writer::notify ();
         }
      }
   }
   catch (adt::RuntimeException& ex) {
      std::cerr << ex.what() << std::endl;
   }
}

// When there is some kind of error over the stream, it will only trace error's
bool logger::CircularTraceWriter::isActive (const logger::Level::_v level) const
   throw ()
{
   return (m_stream != NullStream) ? logger::Writer::isActive(level): level <= Level::Error;
}

void wepa::logger::CircularTraceWriter::openStream()
   throw (adt::RuntimeException)
{
   int stream = open (m_path.c_str (), O_RDWR | O_CREAT | O_APPEND, S_IRUSR |S_IWUSR | S_IRGRP| S_IROTH);

   if (stream == -1)
      WEPA_THROW_EXCEPTION("Can not open file: " << m_path << ". Error: " << strerror(errno));

   m_stream = stream;

   fcntl (stream, F_SETFL, fcntl (stream, F_GETFL) | O_NONBLOCK);
}

bool wepa::logger::CircularTraceWriter::oversizedStream()
   throw (adt::RuntimeException)
{
   if (m_stream == NullStream)
      return false;

   struct stat data;
   int r;

   if (fstat (m_stream, &data) == -1)
      WEPA_THROW_EXCEPTION("Can not get file length: " << m_path << ". Error: " << strerror(errno));

   return data.st_size > m_maxSize;
}

void logger::CircularTraceWriter::closeStream()
   throw ()
{
   if (m_stream != NullStream)
      ::close (m_stream);

   m_stream = NullStream;
   m_lineno = 0;
}

void logger::CircularTraceWriter::renameFile()
   throw (adt::RuntimeException)
{
   std::string file_old (m_path);
   file_old.append (".old");

   if (rename (m_path.c_str (), file_old.c_str ()) != 0) {
      int xerrno = errno;
      truncate (m_path.c_str (), 0);
      WEPA_THROW_EXCEPTION("Could not rename: " << m_path << ". Error: " << strerror(xerrno));
   }
}
