// WEPA - Write Excellent Professional Applications
//
//(c) Copyright 2013 Francisco Ruiz Rayo
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#include <boost/test/unit_test.hpp>

#include <wepa/logger/Logger.hpp>
#include <wepa/logger/Formatter.hpp>
#include <wepa/logger/Writer.hpp>

using namespace wepa;

class DummyFormatter : public logger::Formatter {
public:
   DummyFormatter() : logger::Formatter("DummyFormatter") {;}

private:
   const adt::StreamString& do_apply(const Elements& elements, adt::StreamString& output) throw() {
      return output = elements.input;
   }
};

class CounterStrategy : public logger::Writer {
public:
   CounterStrategy() : logger::Writer("CounterStrategy"),m_total(0) {;}

   unsigned int getCounter(const logger::Level::_v level) const throw() { return m_counters [level]; }
   unsigned int getTotal() const throw() { return m_total; }

private:
   unsigned int m_counters [logger::Level::Local7 + 1];
   unsigned int m_total;

   void do_initialize() throw(adt::RuntimeException){
      memset(&m_counters, 0, sizeof(m_counters));
      m_total = 0;
   }

   void apply(const logger::Level::_v level, const std::string& line) throw() {
      m_counters [level] ++;
      ++ m_total;
   }
};


using namespace wepa::logger;

BOOST_AUTO_TEST_CASE(filter_level)
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

   BOOST_REQUIRE_EQUAL(ss->getTotal(), 3);
   BOOST_REQUIRE_EQUAL(ss->getTotal(), second->getTotal());

   BOOST_REQUIRE_EQUAL(ss->getCounter(Level::Error), 1);
   BOOST_REQUIRE_EQUAL(ss->getCounter(Level::Warning), 1);
   BOOST_REQUIRE_EQUAL(ss->getCounter(Level::Notice), 1);
   BOOST_REQUIRE_EQUAL(ss->getCounter(Level::Information), 0);
   BOOST_REQUIRE_EQUAL(ss->getCounter(Level::Debug), 0);

   Logger::setLevel(Level::Information);

   LOG_ERROR("line" << 0)
   LOG_WARN("line" << 1)
   LOG_NOTICE("line" << 2)
   LOG_INFO("line" << 3);
   LOG_DEBUG("line" << 4 << ". This will be filtered");

   BOOST_REQUIRE_EQUAL(ss->getTotal(), 7);
   BOOST_REQUIRE_EQUAL(ss->getTotal(), second->getTotal());

   BOOST_REQUIRE_EQUAL(ss->getCounter(Level::Error), 2);
   BOOST_REQUIRE_EQUAL(ss->getCounter(Level::Warning), 2);
   BOOST_REQUIRE_EQUAL(ss->getCounter(Level::Notice), 2);
   BOOST_REQUIRE_EQUAL(ss->getCounter(Level::Information), 1);
   BOOST_REQUIRE_EQUAL(ss->getCounter(Level::Debug), 0);

   Logger::setLevel(Level::Emergency);

   Logger::write(Level::Alert, "Alert will never be filtered", WEPA_FILE_LOCATION);
   Logger::write(Level::Critical, "Critical will never be filtered", WEPA_FILE_LOCATION);

   LOG_ERROR("line" << 0 << ". Error will never be filtered")
   LOG_WARN("line" << 1 << ". This will be filtered")
   LOG_NOTICE("line" << 2 << ". This will be filtered")
   LOG_INFO("line" << 3 << ". This will be filtered");
   LOG_DEBUG("line" << 4 << ". This will be filtered");

   BOOST_REQUIRE_EQUAL(ss->getTotal(), 10);

   adt::RuntimeException ex("None",WEPA_FILE_LOCATION);

   Logger::write(ex);
   BOOST_REQUIRE_EQUAL(ss->getTotal(), 11);
   BOOST_REQUIRE_EQUAL(ss->getCounter(Level::Error), 4);
}

BOOST_AUTO_TEST_CASE(use_every_level)
{
   auto ss = std::make_shared<CounterStrategy>();
   Logger::initialize(ss, std::make_shared<DummyFormatter>());

   Logger::setLevel(Level::Local7);

   for(int ii = Level::Emergency; ii <= Level::Local7; ++ ii) {
      Logger::write((Level::_v) ii, "this is the line", WEPA_FILE_LOCATION);
   }

   for(int ii = Level::Emergency; ii <= Level::Local7; ++ ii) {
      BOOST_REQUIRE_EQUAL(ss->getCounter((Level::_v) ii), 1U);
   }

   BOOST_REQUIRE_EQUAL(ss->getTotal(), Level::Local7 + 1);
}

