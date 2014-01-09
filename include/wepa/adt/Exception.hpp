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
#ifndef __wepa_adt_Exception_hpp
#define __wepa_adt_Exception_hpp

#include <stdexcept>

namespace wepa {

namespace adt {

class Exception : std::logic_error {
public:
   using std::logic_error::what;

   virtual ~Exception () throw () {;}

   const char* getMethod () const throw () { return m_fromMethod; }
   const char* getFile () const throw () { return m_fromFile; }
   const unsigned getLine () const throw () { return m_fromLine; }

protected:
   Exception (const std::string& str, const char* fromMethod, const char* fromFile, const unsigned fromLine) :
      std::logic_error (str),
      m_fromMethod (fromMethod),
      m_fromFile (fromFile),
      m_fromLine (fromLine)
   {;}

   Exception (const Exception& other) :
      std::logic_error (other),
      m_fromMethod (other.m_fromMethod),
      m_fromFile (other.m_fromFile),
      m_fromLine (other.m_fromLine)
   {;}

   std::string filePosition () const throw ();

private:
   const char* m_fromMethod;
   const char* m_fromFile;
   const unsigned m_fromLine;
};

}
}

#endif
