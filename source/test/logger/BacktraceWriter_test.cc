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

#include <gtest/gtest.h>

#include <iostream>
#include <chrono>

#include <coffee/basis/AsString.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/Formatter.hpp>
#include <coffee/logger/BacktraceWriter.hpp>

using namespace coffee;
using namespace coffee::logger;

struct BacktrakingTraceTest : public ::testing::Test {
   static int backtrackingLength;

   BacktrakingTraceTest() {
      unlink("backtrace.log");
      unlink("backtrace.log.old");

      writer = std::make_shared<BacktraceWriter>("backtrace.log", 256, backtrackingLength);
      Logger::initialize(writer);
   }
   ~BacktrakingTraceTest() {
      unlink("backtrace.log");
      unlink("backtrace.log.old");
   }

   std::shared_ptr<BacktraceWriter> writer;
};

int BacktrakingTraceTest::backtrackingLength = 5;

TEST_F(BacktrakingTraceTest, basic_work)
{
   for(int ii = 0; ii < backtrackingLength * 2; ++ ii)
      LOG_DEBUG("this is the line number " << ii);

   LOG_NOTICE("notice line");
   LOG_INFO("info line");

   Logger::write(Level::Local0, "ignored line", COFFEE_FILE_LOCATION);

   LOG_ERROR("This is the error");

   ASSERT_EQ(backtrackingLength + 1, writer->getLineNo());
}

TEST_F(BacktrakingTraceTest, down_lowest_level_test)
{
   writer->setLowestLeveL(Level::Local0);

   LOG_NOTICE("notice line");
   LOG_INFO("info line");
   LOG_DEBUG("debug line");

   Logger::write(Level::Local0, "local0", COFFEE_FILE_LOCATION);
   Logger::write(Level::Local1, "ignored line", COFFEE_FILE_LOCATION);
   Logger::write(Level::Local2, "ignored line", COFFEE_FILE_LOCATION);

   Logger::write(Level::Alert, "this is the error", COFFEE_FILE_LOCATION);

   ASSERT_EQ(backtrackingLength, writer->getLineNo());
}

TEST_F(BacktrakingTraceTest, up_lowest_level_test)
{
   writer->setLowestLeveL(Level::Information);

   LOG_NOTICE("notice line");
   LOG_INFO("info line");
   LOG_DEBUG("debug line");

   Logger::write(Level::Local0, "local0", COFFEE_FILE_LOCATION);
   Logger::write(Level::Local1, "ignored line", COFFEE_FILE_LOCATION);
   Logger::write(Level::Local2, "ignored line", COFFEE_FILE_LOCATION);

   Logger::write(Level::Alert, "this is the error", COFFEE_FILE_LOCATION);

   ASSERT_EQ(3, writer->getLineNo());
}

TEST_F(BacktrakingTraceTest, change_level_test)
{
   Logger::setLevel(Level::Error);

   for(int ii = 0; ii < backtrackingLength * 2; ++ ii)
      LOG_DEBUG("this is the line number " << ii);

   LOG_NOTICE(__PRETTY_FUNCTION__);
   LOG_INFO(__PRETTY_FUNCTION__);

   Logger::write(Level::Local0, "ignored line", COFFEE_FILE_LOCATION);

   LOG_ERROR(__PRETTY_FUNCTION__);

   ASSERT_EQ(backtrackingLength + 1, writer->getLineNo());
}

TEST_F(BacktrakingTraceTest, performance_measure_test)
{
   Logger::setLevel(Level::Error);

   auto startTime = std::chrono::high_resolution_clock::now();

   Logger::initialize(writer);

   const int maxLine = 1000;

   Level::_v level = Level::Notice;
   for(int ii = 0; ii < maxLine; ++ ii) {
      if(Logger::wantsToProcess(level) == true) {
         basis::StreamString msg;
         Logger::write(level, msg << "Line=" << ii, COFFEE_FILE_LOCATION);
      }

      if((level + 1) == Level::Local0)
         level = Level::Notice;
      else
         level =(Level::_v)(((int) level) + 1);

      if((ii % 100) == 0)
         LOG_ERROR("step " << ii / 100);
   }

   auto endTime = std::chrono::high_resolution_clock::now();

   std::chrono::microseconds elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
   std::cout << "Delay(BacktraceWriter): " << basis::AsString::apply(elapsedTime) << std::endl << std::endl;
}
