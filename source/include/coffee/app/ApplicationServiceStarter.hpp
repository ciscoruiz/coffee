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


#ifndef _coffee_app_ApplicationEngineRunner_hpp_
#define _coffee_app_ApplicationEngineRunner_hpp_

#include <mutex>
#include <thread>
#include <condition_variable>

#include <coffee/basis/Semaphore.hpp>

#include <coffee/app/Application.hpp>

namespace coffee {

namespace xml {
class Node;
}

namespace app {

/**
 * Starts all services attached to this instance and wait for requestStop, then
 * it will stop all services before finished execution.
 *
 * \warning Application::start method should be called from a thread different
 * from which it will call to the requestStop method.
 *
 * \include Application_test.cc
 *
 */
class ApplicationServiceStarter : public Application {
public:
   /**
      Constructor.
      @param shortName Logical name.
   */
   explicit ApplicationServiceStarter(const char* shortName);

   /**
    * Once you call start method from a thread you have to wait for the application really start
    * its execution.
    */
   void waitUntilRunning() noexcept { semaphoreForRun.wait(); }

private:
   basis::Semaphore semaphoreForRun;
   std::mutex mutex;
   std::condition_variable conditionForStop;
   bool stopNow;

   void run() throw(basis::RuntimeException);
   void do_stop() throw(basis::RuntimeException);
};

}
}

#endif

