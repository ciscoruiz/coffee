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

   return data.st_size > m_maxSize;
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
