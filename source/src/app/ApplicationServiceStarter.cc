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

#include "../../include/coffee/app/ApplicationServiceStarter.hpp"

#include <coffee/config/SCCS.hpp>
#include <coffee/logger/TraceMethod.hpp>

using namespace coffee;

coffee_sccs_import_tag(app);

app::ApplicationServiceStarter::ApplicationServiceStarter(const char* shortName):
    app::Application(shortName, "Application for run attached engines", coffee_sccs_use_tag(app)),
    semaphoreForRun(0),
    stopNow(false)
 {
 }

void app::ApplicationServiceStarter::run()
   throw(basis::RuntimeException)
{
   LOG_THIS_METHOD();

   semaphoreForRun.signal();
   std::unique_lock <std::mutex> guard (mutex);
   while(!stopNow) {
      conditionForStop.wait(guard);
   }
}

void app::ApplicationServiceStarter::do_stop()
   throw(basis::RuntimeException)
{
   LOG_THIS_METHOD();

   try {
      app::Application::do_stop();
      stopNow = true;
      conditionForStop.notify_all();
   }
   catch(basis::RuntimeException&) {
      stopNow = true;
      conditionForStop.notify_all();
      throw;
   }
}
