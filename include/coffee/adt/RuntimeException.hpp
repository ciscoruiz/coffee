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

#ifndef _coffee_adt_RuntimeException_h
#define _coffee_adt_RuntimeException_h

#include <sstream>

#include <coffee/adt/StreamString.hpp>
#include <coffee/adt/Exception.hpp>

namespace coffee {

namespace adt {

/**
 * Defines exception used for this library.
 *
 * @see http://www.boost.org/doc/libs/1_39_0/libs/exception/doc/exception_types_as_simple_semantic_tags.html
 */
class RuntimeException : public Exception {
public:
   static const int NullErrorCode = -1;

   RuntimeException (const std::string& str, const char* fromMethod, const char* fromFile, const unsigned fromLine) :
      Exception (str, fromMethod, fromFile, fromLine),
      m_errorCode (NullErrorCode)
   {;}

   RuntimeException (const RuntimeException& other) :
      Exception (other),
      m_errorCode (other.m_errorCode)
   {;}

   int getErrorCode () const noexcept { return m_errorCode; }

   void setErrorCode (const int errorCode) noexcept { m_errorCode = errorCode; }

   std::string asString () const noexcept;

private:
   int m_errorCode;
};

}
}

#define COFFEE_THROW_EXCEPTION(msg) do { coffee::adt::StreamString __str; __str << msg; throw coffee::adt::RuntimeException (__str, __PRETTY_FUNCTION__, __FILE__, __LINE__); } while (false)
#define COFFEE_THROW_NAMED_EXCEPTION(name,msg) do { coffee::adt::StreamString __str; __str << msg; throw name(__str, __PRETTY_FUNCTION__, __FILE__, __LINE__); } while (false)


#endif // _coffee_adt_RuntimeException_h
