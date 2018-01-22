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
#include <boost/test/unit_test.hpp>

#include <iostream>

#include <wepa/adt/Microsecond.hpp>

#include <wepa/logger/Logger.hpp>
#include <wepa/logger/Formatter.hpp>
#include <wepa/logger/BacktraceWriter.hpp>

using namespace wepa;
using namespace wepa::logger;

BOOST_AUTO_TEST_CASE (backtraking_test)
{
   int backtrackingLength = 5;

   auto writer = std::make_shared<BacktraceWriter>("backtrace.log", 256, backtrackingLength);
   Logger::initialize (writer);

   for (int ii = 0; ii < backtrackingLength * 2; ++ ii)
      LOG_DEBUG ("this is the line number " << ii);

   LOG_NOTICE ("notice line");
   LOG_INFO ("info line");

   Logger::write(Level::Local0, "ignored line", WEPA_FILE_LOCATION);

   LOG_ERROR ("This is the error");

   BOOST_REQUIRE_EQUAL(writer->getLineNo(), backtrackingLength + 1);
}

BOOST_AUTO_TEST_CASE (backtraking_down_lowest_level_test)
{
   int backtrackingLength = 5;

   auto writer = std::make_shared<BacktraceWriter>("backtrace.log", 256, backtrackingLength);
   Logger::initialize (writer);

   writer->setLowestLeveL(Level::Local0);

   LOG_NOTICE ("notice line");
   LOG_INFO ("info line");
   LOG_DEBUG ("debug line");

   Logger::write(Level::Local0, "local0", WEPA_FILE_LOCATION);
   Logger::write(Level::Local1, "ignored line", WEPA_FILE_LOCATION);
   Logger::write(Level::Local2, "ignored line", WEPA_FILE_LOCATION);

   Logger::write(Level::Alert, "this is the error", WEPA_FILE_LOCATION);

   BOOST_REQUIRE_EQUAL(writer->getLineNo(), backtrackingLength);
}

BOOST_AUTO_TEST_CASE (backtraking_up_lowest_level_test)
{
   int backtrackingLength = 5;

   auto writer = std::make_shared<BacktraceWriter>("backtrace.log", 256, backtrackingLength);
   Logger::initialize (writer);

   writer->setLowestLeveL(Level::Information);

   LOG_NOTICE ("notice line");
   LOG_INFO ("info line");
   LOG_DEBUG ("debug line");

   Logger::write(Level::Local0, "local0", WEPA_FILE_LOCATION);
   Logger::write(Level::Local1, "ignored line", WEPA_FILE_LOCATION);
   Logger::write(Level::Local2, "ignored line", WEPA_FILE_LOCATION);

   Logger::write(Level::Alert, "this is the error", WEPA_FILE_LOCATION);

   BOOST_REQUIRE_EQUAL(writer->getLineNo(), 3);
}

BOOST_AUTO_TEST_CASE (backtraking_change_level_test)
{
   int backtrackingLength = 5;

   auto writer = std::make_shared<BacktraceWriter>("backtrace.log", 256, backtrackingLength);
   Logger::initialize (writer);

   Logger::setLevel (Level::Error);

   for (int ii = 0; ii < backtrackingLength * 2; ++ ii)
      LOG_DEBUG ("this is the line number " << ii);

   LOG_NOTICE (__PRETTY_FUNCTION__);
   LOG_INFO (__PRETTY_FUNCTION__);

   Logger::write(Level::Local0, "ignored line", WEPA_FILE_LOCATION);

   LOG_ERROR (__PRETTY_FUNCTION__);

   BOOST_REQUIRE_EQUAL(writer->getLineNo(), backtrackingLength + 1);
}

BOOST_AUTO_TEST_CASE (backtraking_performance_measure_test)
{
   int backtrackingLength = 32;

   auto writer = std::make_shared<BacktraceWriter>("backtrace.log", 256, backtrackingLength);

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
