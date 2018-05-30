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
   void initialize () throw (basis::RuntimeException) {;}
};


TEST( DefaultFormatter,_basic )
{
   auto writer = std::make_shared<TtyWriter>();
   logger::Logger::initialize(writer);

   logger::Logger::setLevel (logger::Level::Local2);

   basis::StreamString msg;
   for (int ii = logger::Level::Emergency; ii <= logger::Level::Local7; ++ ii) {
      msg.clear ();
      logger::Logger::write((logger::Level::_v)ii, msg << " numeric value=" << ii, COFFEE_FILE_LOCATION);
   }

   ASSERT_EQ(logger::Level::Local2 + 1, writer->getCounter ());
}

