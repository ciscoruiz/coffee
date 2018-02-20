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

#include <coffee/adt/StreamString.hpp>
#include <coffee/adt/NamedObject.hpp>

#include <coffee/logger/Level.hpp>

namespace coffee {

namespace logger {

class Logger;

class Formatter : public adt::NamedObject {
public:
   struct Elements {
      const Level::_v level;
      const adt::StreamString& input;
      const char* function;
      const char* file;
      const unsigned lineno;

      Elements (const Level::_v _level, const adt::StreamString& _input, const char* _function, const char* _file, const unsigned _lineno) :
         level (_level), input (_input), function (_function), file (_file), lineno (_lineno)
      {;}
   };

   virtual ~Formatter () {;}

protected:
   explicit Formatter (const std::string& name) : adt::NamedObject (name) {;}

   virtual std::string apply (const Elements& elements) noexcept = 0;

private:
   friend class Logger;

   Formatter (const Formatter&);
};

}
}

#endif
