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

#ifndef coffee_logger_Writer_hpp
#define coffee_logger_Writer_hpp

#include <coffee/logger/Level.hpp>
#include <coffee/adt/RuntimeException.hpp>

namespace coffee {

namespace logger {

class Logger;

/**
 * Generic writer of traces.
 */
class Writer {
public:
   /**
    * Destructor.
    */
   virtual ~Writer() {;}

protected:
   /**
    * constructor.
    * \name Logical name of the Writer.
    */
   explicit Writer(const std::string& name) : m_name(name) {;}

   /**
    * \return \b true if the writer wants to process the trace with the received level or \b false otherwise.
    */
   virtual bool wantsToProcess(const Level::_v level) const noexcept;

private:
   const std::string m_name;

   Writer(const Writer&);

   virtual void initialize() throw(adt::RuntimeException) = 0;
   virtual void apply(const Level::_v level, const std::string& line) noexcept = 0;

   friend class Logger;
};

}
}

#endif
