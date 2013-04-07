// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2013 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/wepa
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
#ifndef _wepa_adt_RuntimeException_h
#define _wepa_adt_RuntimeException_h

#include <boost/exception/exception.hpp>
#include <stdexcept>
#include <sstream>

#include <wepa/adt/StreamString.hpp>

namespace wepa {

namespace adt {

/**
 * Defines exception used for this library.
 *
 * @see http://www.boost.org/doc/libs/1_39_0/libs/exception/doc/exception_types_as_simple_semantic_tags.html
 */
class RuntimeException : virtual std::logic_error, virtual public boost::exception {
public:
   static const int NullErrorCode = -1;

   using std::logic_error::what;

   explicit RuntimeException (const std::string& str, const char* fromMethod, const char* fromFile, const unsigned fromLine) :
      std::logic_error (str),
      m_fromMethod (fromMethod),
      m_fromFile (fromFile),
      m_fromLine (fromLine),
      m_errorCode (NullErrorCode)
   {;}

   RuntimeException (const RuntimeException& other) :
      std::logic_error (other),
      m_fromMethod (other.m_fromMethod),
      m_fromFile (other.m_fromFile),
      m_fromLine (other.m_fromLine),
      m_errorCode (other.m_errorCode)
   {;}

   int getErrorCode () const throw () { return m_errorCode; }

   void setErrorCode (const int errorCode) throw () { m_errorCode = errorCode; }

   std::string asString () const throw ();

private:
   const char* m_fromMethod;
   const char* m_fromFile;
   const unsigned m_fromLine;
   int m_errorCode;
};

}
}

#define WEPA_THROW_EXCEPTION(msg) do { wepa::adt::StreamString str; str << msg; throw wepa::adt::RuntimeException (str, __PRETTY_FUNCTION__, __FILE__, __LINE__); } while (false)


#endif // _wepa_adt_RuntimeException_h