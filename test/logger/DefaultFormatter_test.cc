// COFFEE - COmpany eFFEEctive Platform
//
// (c) Copyright 2013 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/coffee
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

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/Writer.hpp>
#include <coffee/logger/DefaultFormatter.hpp>

#include <iostream>

using namespace coffee;

class TtyWriter : public logger::Writer {
public:
   TtyWriter () : logger::Writer ("TtyWriter"), m_counter (0) {;}

   unsigned getCounter () const throw () { return m_counter; }

private:
   unsigned m_counter;

   void apply (const logger::Level::_v level, const std::string& line) throw () {
      std::cout << line << std::endl;
      ++ m_counter;
   }
   void do_initialize () throw (adt::RuntimeException) {;}
};


BOOST_AUTO_TEST_CASE( DefaultFormatter_test )
{
   auto writer = std::make_shared<TtyWriter>();
   logger::Logger::initialize(writer);

   logger::Logger::setLevel (logger::Level::Local2);

   adt::StreamString msg;
   for (int ii = logger::Level::Emergency; ii <= logger::Level::Local7; ++ ii) {
      msg.clear ();
      logger::Logger::write((logger::Level::_v)ii, msg << " numeric value=" << ii, COFFEE_FILE_LOCATION);
   }

   BOOST_REQUIRE_EQUAL(writer->getCounter (), logger::Level::Local2 + 1);
}

