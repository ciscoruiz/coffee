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

#ifndef __coffee_logger_CircularTraceWriter_hpp
#define __coffee_logger_CircularTraceWriter_hpp

#include <coffee/logger/Writer.hpp>

namespace coffee {

namespace logger {

class CircularTraceWriter : public Writer {
public:
   static const int MinimalSize = 256;
   static const int NullStream;
   static const int CheckSizePeriod;

   CircularTraceWriter (const std::string& path, const size_t maxSize);
   virtual ~CircularTraceWriter () { closeStream (); }

   int getStream () const noexcept { return m_stream; }
   size_t getLineNo () const noexcept { return m_lineno; }
   size_t getKbytesMaxSize () const noexcept { return m_maxSize; }

protected:
   virtual void apply (const Level::_v level, const std::string& line) noexcept;
   virtual bool wantsToProcess (const Level::_v level) const noexcept;

private:
   std::string m_path;
   size_t m_maxSize;
   int m_stream;
   size_t m_lineno;

   void do_initialize () throw (adt::RuntimeException);

   void openStream () throw (adt::RuntimeException);
   bool oversizedStream () throw (adt::RuntimeException);
   void closeStream () noexcept;
   void renameFile () throw (adt::RuntimeException);
};

}
}

#endif
