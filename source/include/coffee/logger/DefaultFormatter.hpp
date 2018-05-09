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

#ifndef __coffee_logger_DefaultFormatter_hpp
#define __coffee_logger_DefaultFormatter_hpp

#include <coffee/logger/Formatter.hpp>
#include <memory>

namespace coffee {
namespace logger {

/**
 * Default formatter use in case no other is defined. The output will look like the next lines:
 *
 * \code
[23/02/2018 10:38:33] Alert | void DefaultFormatter_test::test_method() [coffee/test/logger/DefaultFormatter_test.cc(61)]:  numeric value=1
[23/02/2018 10:38:33] Critical | void DefaultFormatter_test::test_method() [coffee/test/logger/DefaultFormatter_test.cc(61)]:  numeric value=2
[23/02/2018 10:38:33] Error | void DefaultFormatter_test::test_method() [coffee/test/logger/DefaultFormatter_test.cc(61)]:  numeric value=3
[23/02/2018 10:38:33] Warning | void DefaultFormatter_test::test_method() [coffee/test/logger/DefaultFormatter_test.cc(61)]:  numeric value=4
[23/02/2018 10:38:33] Notice | void DefaultFormatter_test::test_method() [coffee/test/logger/DefaultFormatter_test.cc(61)]:  numeric value=5
[23/02/2018 10:38:33] Information | void DefaultFormatter_test::test_method() [coffee/test/logger/DefaultFormatter_test.cc(61)]:  numeric value=6
\endcode
 */
class DefaultFormatter : public Formatter {
public:
   /**
    * Constructor.
    */
   DefaultFormatter() {;}

   /**
    * Fast shared creator
    */
   static std::shared_ptr<DefaultFormatter> instantiate() {
      return std::make_shared<DefaultFormatter>();
   }

private:
   std::string apply(const Level::_v level, const basis::StreamString& comment, const char* methodName, const char* file, const unsigned lineno) noexcept;

};

}
}
#endif
