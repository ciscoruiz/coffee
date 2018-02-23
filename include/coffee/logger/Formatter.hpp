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

#ifndef __coffee_logger_Formatter_hpp
#define __coffee_logger_Formatter_hpp

#include <coffee/logger/Level.hpp>

namespace coffee {

namespace logger {

class Logger;

/**
 * Generic formatter to generate the final trace that will be used by the logger::Writer
 */
class Formatter {
public:
   /**
    * Destructor.
    */
   virtual ~Formatter() {;}

protected:
   /**
    * Constructor
    */
   Formatter() {;}

   /**
    * Pure virtual method to combine parameters into the std::string to get the final trace.
    * \return The string to be used by writer(s).
    *
    * \param level Active Level with the trace was generated
    * \param comment Contains the message to be used
    * \param methodName method name where the trace was created
    * \param file The file where the trace was created
    * \param lineno The line number where the trace was created.
    */
   virtual std::string apply(const Level::_v level, const adt::StreamString& comment, const char* methodName, const char* file, const unsigned lineno) noexcept = 0;

private:
   friend class Logger;

   Formatter (const Formatter&);
};

}
}

#endif
