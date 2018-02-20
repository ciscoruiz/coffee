#include <boost/test/unit_test.hpp>

#include <iostream>

#include <coffee/adt/Microsecond.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/Formatter.hpp>
#include <coffee/logger/BacktraceWriter.hpp>

using namespace coffee;
using namespace coffee::logger;

BOOST_AUTO_TEST_CASE (backtraking_test)
{
   int backtrackingLength = 5;

   auto writer = std::make_shared<BacktraceWriter>("backtrace.log", 256, backtrackingLength);
   Logger::initialize (writer);

   for (int ii = 0; ii < backtrackingLength * 2; ++ ii)
      LOG_DEBUG ("this is the line number " << ii);

   LOG_NOTICE ("notice line");
   LOG_INFO ("info line");

   Logger::write(Level::Local0, "ignored line", COFFEE_FILE_LOCATION);

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

   Logger::write(Level::Local0, "local0", COFFEE_FILE_LOCATION);
   Logger::write(Level::Local1, "ignored line", COFFEE_FILE_LOCATION);
   Logger::write(Level::Local2, "ignored line", COFFEE_FILE_LOCATION);

   Logger::write(Level::Alert, "this is the error", COFFEE_FILE_LOCATION);

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

   Logger::write(Level::Local0, "local0", COFFEE_FILE_LOCATION);
   Logger::write(Level::Local1, "ignored line", COFFEE_FILE_LOCATION);
   Logger::write(Level::Local2, "ignored line", COFFEE_FILE_LOCATION);

   Logger::write(Level::Alert, "this is the error", COFFEE_FILE_LOCATION);

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

   Logger::write(Level::Local0, "ignored line", COFFEE_FILE_LOCATION);

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
         Logger::write (level, msg << "Line=" << ii, COFFEE_FILE_LOCATION);
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
