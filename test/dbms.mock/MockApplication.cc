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

#include "MockApplication.hpp"

#include <iostream>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TraceMethod.hpp>
#include <coffee/logger/TtyWriter.hpp>

using namespace coffee;

mock::MockApplication::MockApplication (const char* title) : app::Application ("MockApp", title, "0.0")
{
   logger::Logger::initialize(std::make_shared<logger::TtyWriter>());
}

void mock::MockApplication::operator ()()
{
   try {
      start ();
   }
   catch (adt::Exception& ex) {
      logger::Logger::write(ex);
   }
}

void mock::MockApplication::run ()
   throw (adt::RuntimeException)
{
   LOG_THIS_METHOD();

   LOG_DEBUG (asString() << "Waiting for enabled termination");
   m_termination.lock();
   LOG_DEBUG (asString() << " will terminate now");

   // It will be block until something frees the mutex by calling enableTermination
}
