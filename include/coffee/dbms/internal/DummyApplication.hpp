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

#ifndef __coffee_dbms_internal_DummyApplication_hpp
#define __coffee_dbms_internal_DummyApplication_hpp

#include <functional>

#include <coffee/app/Application.hpp>

namespace coffee {

namespace app {
class Engine;
}

namespace dbms {
namespace internal {

class DummyApplication : public app::Application {
public:
   DummyApplication () : app::Application ("dummy", "dummy", "1.0") {;}

   static app::Application& getInstance () noexcept { return std::ref (st_this); }

private:
   static DummyApplication st_this;

   void run () throw (adt::RuntimeException) {;}
};


} /* namespace internal */
} /* namespace dbms */
} /* namespace coffee */
#endif
