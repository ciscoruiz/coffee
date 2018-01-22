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
#include <boost/test/auto_unit_test.hpp>

#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <iostream>

#include <wepa/adt/Microsecond.hpp>

#include <wepa/logger/CircularTraceWriter.hpp>
#include <wepa/logger/Logger.hpp>

#include <wepa/adt/pattern/observer/Observer.hpp>

using namespace wepa;
using namespace wepa::logger;


class TraceObserver : public adt::pattern::observer::Observer {
public:
   TraceObserver () : adt::pattern::observer::Observer ("TraceObserver") , m_initCounter (0) {;}

   int getInitCounter () const throw () { return m_initCounter; }

private:
   int m_initCounter;

   void update (const adt::pattern::observer::Event&) throw () {
      ++ m_initCounter;
   }
};

BOOST_AUTO_TEST_CASE( CircularTraceWriter_oversized_file )
{
   unlink ("trace.log");
   unlink ("trace.log.old");

   auto writer = std::make_shared<CircularTraceWriter>("trace.log", 128);
   Logger::initialize (writer);

   Logger::setLevel(Level::Debug);

   BOOST_REQUIRE_NE (writer->getStream(), CircularTraceWriter::NullStream);

   std::string value (1024, 'x');

   int loop = writer->getKbytesMaxSize() / value.size ();

   std::cout << "It will need " << loop << " loops to initialize the file trace" << std::endl;

   for (int ii = 0; ii < loop; ++ ii) {
      LOG_DEBUG(ii << " " << value);
   }

   LOG_DEBUG ("This line will generate an oversized file while the size is measured");

   for (int ii = 0; ii < loop; ++ ii) {
      LOG_DEBUG(ii << " " << value);
   }

   LOG_DEBUG ("This line will generate an oversized file while the size is measured");

   BOOST_REQUIRE_EQUAL (writer->getLineNo(), loop * 2 + 2);

   int stream = open ("trace.log.old", O_RDWR | O_CREAT | O_EXCL, S_IRUSR |S_IWUSR | S_IRGRP| S_IROTH);

   BOOST_REQUIRE (stream == -1 && errno == EEXIST);
}

BOOST_AUTO_TEST_CASE( CircularTraceWriter_can_not_write )
{
   auto writer = std::make_shared<CircularTraceWriter>("/trace.log", 128);

   // When the circular writer can not write over the file, then it will trace on cerr, but only traces with level error or lesser
   BOOST_CHECK_THROW (Logger::initialize(writer), adt::RuntimeException);

   BOOST_REQUIRE_EQUAL (writer->getStream(), CircularTraceWriter::NullStream);

   Logger::write (Level::Emergency, "eee", WEPA_FILE_LOCATION);
   Logger::write (Level::Alert, "AAA", WEPA_FILE_LOCATION);
   Logger::write (Level::Critical, "CCC", WEPA_FILE_LOCATION);
   Logger::write (Level::Error, "ee", WEPA_FILE_LOCATION);

   LOG_WARN ("Ignored line");
   LOG_NOTICE ("Ignored line");
   LOG_INFO ("Ignored line");
   LOG_DEBUG ("Ignored line");
   LOG_LOCAL7("Ignored line");

   BOOST_REQUIRE_EQUAL (writer->getLineNo(), 4);
}

BOOST_AUTO_TEST_CASE (circular_performance_measure_test)
{
   auto writer = std::make_shared<CircularTraceWriter>("trace.log", 4096);

   Logger::setLevel (Level::Error);

   adt::Microsecond init = adt::Microsecond::getTime();

   Logger::initialize(writer);

   const int maxLine = 1000;

   Level::_v level = Level::Notice;
   for (int ii = 0; ii < maxLine; ++ ii) {
      if (Logger::wantsToProcess(level) == true) {
         adt::StreamString msg;
         Logger::write (level, msg << "Line=" << ii, WEPA_FILE_LOCATION);
      }

      if ((level + 1) == Level::Local0)
         level = Level::Notice;
      else
         level = (Level::_v) (((int) level) + 1);

      if ((ii % 100) == 0)
         LOG_ERROR ("step " << ii / 100);
   }

   adt::Microsecond end = adt::Microsecond::getTime();

   std::cout << "Delay: " << end - init << " ms" << std::endl << std::endl;
}
