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

#include <boost/test/auto_unit_test.hpp>

#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <iostream>
#include <chrono>

#include <coffee/basis/AsString.hpp>

#include <coffee/logger/CircularTraceWriter.hpp>
#include <coffee/logger/Logger.hpp>

#include <coffee/basis/pattern/observer/Observer.hpp>

using namespace coffee;
using namespace coffee::logger;

struct CircularTraceFixture {
   CircularTraceFixture() {
      unlink ("trace.log");
      unlink ("trace.log.old");

      Logger::setLevel(Level::Debug);

   }
   ~CircularTraceFixture() {
      unlink ("trace.log");
      unlink ("trace.log.old");
   }

   void fillup(std::shared_ptr<CircularTraceWriter>& writer) {
      std::string value (1024, 'x');
      int loop = writer->getKbytesMaxSize() / value.size ();
      std::cout << "It will need " << loop << " loops to initialize the file trace" << std::endl;
      for (int ii = 0; ii < loop; ++ ii) {
         LOG_DEBUG(ii << " " << value);
      }
   }
};

BOOST_FIXTURE_TEST_CASE(CircularTraceWriter_oversized_file, CircularTraceFixture)
{
   auto writer = std::make_shared<CircularTraceWriter>("trace.log", 128);
   Logger::initialize (writer);

   BOOST_REQUIRE_NE (writer->getStream(), CircularTraceWriter::NullStream);

   BOOST_REQUIRE_NO_THROW(fillup(writer));
   LOG_DEBUG ("This line will generate an oversized file while the size is measured");
   BOOST_REQUIRE_EQUAL(writer->getLoops(), 1);

   BOOST_REQUIRE_NO_THROW(fillup(writer));
   LOG_DEBUG ("This line will generate an oversized file while the size is measured");
   BOOST_REQUIRE_EQUAL(writer->getLoops(), 2);

   int stream = open ("trace.log.old", O_RDWR | O_CREAT | O_EXCL, S_IRUSR |S_IWUSR | S_IRGRP| S_IROTH);

   BOOST_REQUIRE (stream == -1 && errno == EEXIST);
}

BOOST_FIXTURE_TEST_CASE(CircularTraceWriter_startwith_oversized_file, CircularTraceFixture)
{
   std::ofstream create("trace.log");
   create.close();
   BOOST_REQUIRE_EQUAL(truncate("trace.log", (CircularTraceWriter::MinimalKbSize + 1) * 1024), 0);

   std::shared_ptr<CircularTraceWriter> writer = std::make_shared<CircularTraceWriter>("trace.log", 128);
   BOOST_REQUIRE_NO_THROW(Logger::initialize (writer));
   BOOST_REQUIRE_EQUAL(writer->getLoops(), 1);

   fillup(writer);
   LOG_DEBUG ("This line will generate an oversized file while the size is measured");
   BOOST_REQUIRE_EQUAL(writer->getLoops(), 2);
}

BOOST_AUTO_TEST_CASE( CircularTraceWriter_can_not_write )
{
   auto writer = std::make_shared<CircularTraceWriter>("/trace.log", 128);

   // When the circular writer can not write over the file, then it will trace on cerr, but only traces with level error or lesser
   BOOST_CHECK_THROW (Logger::initialize(writer), basis::RuntimeException);

   BOOST_REQUIRE_EQUAL (writer->getStream(), CircularTraceWriter::NullStream);

   Logger::write (Level::Emergency, "eee", COFFEE_FILE_LOCATION);
   Logger::write (Level::Alert, "AAA", COFFEE_FILE_LOCATION);
   Logger::write (Level::Critical, "CCC", COFFEE_FILE_LOCATION);
   Logger::write (Level::Error, "ee", COFFEE_FILE_LOCATION);

   LOG_WARN ("Ignored line");
   LOG_NOTICE ("Ignored line");
   LOG_INFO ("Ignored line");
   LOG_DEBUG ("Ignored line");
   LOG_LOCAL7("Ignored line");

   BOOST_REQUIRE_EQUAL (writer->getLineNo(), 4);
}

BOOST_FIXTURE_TEST_CASE(circular_performance_measure_test, CircularTraceFixture)
{
   auto writer = std::make_shared<CircularTraceWriter>("trace.log", 4096);

   Logger::setLevel (Level::Error);

   auto startTime = std::chrono::high_resolution_clock::now();

   Logger::initialize(writer);

   const int maxLine = 1000;

   Level::_v level = Level::Notice;
   for (int ii = 0; ii < maxLine; ++ ii) {
      if (Logger::wantsToProcess(level) == true) {
         basis::StreamString msg;
         Logger::write (level, msg << "Line=" << ii, COFFEE_FILE_LOCATION);
      }

      if ((level + 1) == Level::Local0)
         level = Level::Notice;
      else
         level = (Level::_v) (((int) level) + 1);

      if ((ii % 100) == 0)
         LOG_ERROR ("step " << ii / 100);
   }

   auto endTime = std::chrono::high_resolution_clock::now();

   std::chrono::microseconds elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
   std::cout << "Delay(CircularTraceWriter): " << basis::AsString::apply(elapsedTime) << std::endl << std::endl;
}