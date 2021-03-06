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

#include <gtest/gtest.h>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/Formatter.hpp>
#include <coffee/logger/Writer.hpp>

using namespace coffee;

class DummyFormatter : public logger::Formatter {
public:
   DummyFormatter() {;}

private:
   std::string apply(const logger::Level::_v level, const basis::StreamString& comment, const char* methodName, const char* file, const unsigned lineno) noexcept {
      return comment;
   }
};

class CounterStrategy : public logger::Writer {
public:
   CounterStrategy() : logger::Writer("CounterStrategy"),m_total(0) { coffee_memset(m_counters, 0, sizeof(m_counters));}

   unsigned int getCounter(const logger::Level::_v level) const throw() { return m_counters [level]; }
   unsigned int getTotal() const throw() { return m_total; }

private:
   unsigned int m_counters [logger::Level::Local7 + 1];
   unsigned int m_total;

   void initialize() throw(basis::RuntimeException){
      memset(&m_counters, 0, sizeof(m_counters));
      m_total = 0;
   }

   void apply(const logger::Level::_v level, const std::string& line) throw() {
      m_counters [level] ++;
      ++ m_total;
   }
};


using namespace coffee::logger;

TEST(FilteringLogTest,filter_level)
{
   auto ss = std::make_shared<CounterStrategy>();
   auto second = std::make_shared<CounterStrategy>();

   Logger::initialize(ss, std::make_shared<DummyFormatter>());
   Logger::add(second);

   Logger::setLevel(Level::Notice);

   LOG_ERROR("line" << 0)
   LOG_WARN("line" << 1)
   LOG_NOTICE("line" << 2)
   LOG_INFO("line" << 3 << ". This will be filtered");
   LOG_DEBUG("line" << 4 << ". This will be filtered");

   ASSERT_EQ(3, ss->getTotal());
   ASSERT_EQ(second->getTotal(), ss->getTotal());

   ASSERT_EQ(1, ss->getCounter(Level::Error));
   ASSERT_EQ(1, ss->getCounter(Level::Warning));
   ASSERT_EQ(1, ss->getCounter(Level::Notice));
   ASSERT_EQ(0, ss->getCounter(Level::Information));
   ASSERT_EQ(0, ss->getCounter(Level::Debug));

   Logger::setLevel(Level::Information);

   LOG_ERROR("line" << 0)
   LOG_WARN("line" << 1)
   LOG_NOTICE("line" << 2)
   LOG_INFO("line" << 3);
   LOG_DEBUG("line" << 4 << ". This will be filtered");

   ASSERT_EQ(7, ss->getTotal());
   ASSERT_EQ(second->getTotal(), ss->getTotal());

   ASSERT_EQ(2, ss->getCounter(Level::Error));
   ASSERT_EQ(2, ss->getCounter(Level::Warning));
   ASSERT_EQ(2, ss->getCounter(Level::Notice));
   ASSERT_EQ(1, ss->getCounter(Level::Information));
   ASSERT_EQ(0, ss->getCounter(Level::Debug));

   Logger::setLevel(Level::Emergency);

   Logger::write(Level::Alert, "Alert will never be filtered", COFFEE_FILE_LOCATION);
   Logger::write(Level::Critical, "Critical will never be filtered", COFFEE_FILE_LOCATION);

   LOG_ERROR("line" << 0 << ". Error will never be filtered")
   LOG_WARN("line" << 1 << ". This will be filtered")
   LOG_NOTICE("line" << 2 << ". This will be filtered")
   LOG_INFO("line" << 3 << ". This will be filtered");
   LOG_DEBUG("line" << 4 << ". This will be filtered");

   ASSERT_EQ(10, ss->getTotal());

   basis::RuntimeException ex("None",COFFEE_FILE_LOCATION);

   Logger::write(ex);
   ASSERT_EQ(11, ss->getTotal());
   ASSERT_EQ(4, ss->getCounter(Level::Error));
}

TEST(FilteringLogTest,use_every_level)
{
   auto ss = std::make_shared<CounterStrategy>();
   Logger::initialize(ss, std::make_shared<DummyFormatter>());

   Logger::setLevel(Level::Local7);

   for(int ii = Level::Emergency; ii <= Level::Local7; ++ ii) {
      Logger::write((Level::_v) ii, "this is the line", COFFEE_FILE_LOCATION);
   }

   for(int ii = Level::Emergency; ii <= Level::Local7; ++ ii) {
      ASSERT_EQ(1U, ss->getCounter((Level::_v) ii));
   }

   ASSERT_EQ(Level::Local7 + 1, ss->getTotal());
}

