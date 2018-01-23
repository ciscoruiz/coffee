// COFFEE - COmpany eFFEEctive Platform
//
// (c) Copyright 2018 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/coffee
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
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
