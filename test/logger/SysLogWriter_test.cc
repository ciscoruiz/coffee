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

#include <coffee/logger/SysLogWriter.hpp>
#include <coffee/logger/Logger.hpp>

#include <syslog.h>

using namespace coffee;
using namespace coffee::logger;

BOOST_AUTO_TEST_CASE (syslog_option_builder)
{
   SysLogWriter::OptionBuilder option;

   int flags = option.consoleWhenError().noDelayOpen().delayOpen().showPid().logToStdErr().build();
   BOOST_REQUIRE_EQUAL(flags, LOG_CONS + LOG_ODELAY + LOG_NDELAY + LOG_PID + LOG_PERROR);
}

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




