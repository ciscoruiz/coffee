#include <boost/test/auto_unit_test.hpp>

#include <coffee/logger/SysLogWriter.hpp>
#include <coffee/logger/Logger.hpp>

#include <syslog.h>

using namespace coffee;
using namespace coffee::logger;

BOOST_AUTO_TEST_CASE (syslog_test)
{
   SysLogWriter::OptionBuilder option;

   auto writer = std::make_shared<SysLogWriter>("SysLogTest", option.consoleWhenError().noDelayOpen());
   Logger::initialize (writer);

   Logger::setLevel(Level::Debug);

   for (int ii = Level::Emergency; ii < Level::Local7; ++ ii) {
      adt::StreamString ss;
      Level::_v level = (Level::_v) ii;
      ss << "This is the level " << level;
      Logger::write(level, ss, COFFEE_FILE_LOCATION);
   }
}




