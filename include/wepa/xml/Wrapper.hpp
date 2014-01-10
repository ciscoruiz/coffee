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
#ifndef __wepa_xml_Wrapper_hpp
#define __wepa_xml_Wrapper_hpp

#include <boost/function.hpp>

#include <wepa/adt/RuntimeException.hpp>

namespace wepa {

namespace xml {

class Compiler;

template <typename _Handler> class  Wrapper {
public:
   typedef boost::function <void (_Handler*)> Deleter;
   typedef _Handler* Handler;
   typedef boost::function <const char* (const Handler)> NameExtractor;

   virtual ~Wrapper () {
      releaseHandler ();
   }

   Handler getHandler () noexcept { return m_handler; }
   Handler getHandler () const noexcept { return m_handler; }

   const std::string& getName () const {
      if (m_handler == NULL) {
         m_name.clear ();
      }
      else if (m_name.empty() == true) {
         m_name = m_nameExtractor(m_handler);
      }

      return m_name;
   }

   operator Handler () noexcept { return m_handler; }

protected:
   Wrapper () : m_handler (NULL), m_deleter (NULL), m_nameExtractor (NULL) {}
   Wrapper (Handler handler) : m_handler (handler), m_deleter (NULL), m_nameExtractor (NULL) {}

   void setDeleter (Deleter deleter) noexcept { m_deleter = deleter; }
   void setNameExtractor (NameExtractor nameExtractor) noexcept { m_nameExtractor = nameExtractor; }
   Handler setHandler (Handler handler) noexcept { m_handler = handler; m_name.clear (); return m_handler; }

   void releaseHandler ()
      noexcept
   {
      if (m_handler != NULL) {
         if (m_deleter) {
            m_deleter (m_handler);
         }
         m_handler = NULL;
         m_name.clear ();
      }
   }

   virtual void compile (Compiler& compiler) const throw (adt::RuntimeException) {;} ;

private:
   Handler m_handler;
   NameExtractor m_nameExtractor;
   Deleter m_deleter;
   mutable std::string m_name;
};

}

}

#endif
