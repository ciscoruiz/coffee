// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2018 Francisco Ruiz Rayo
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
#ifndef __wepa_xml_DTD_hpp
#define __wepa_xml_DTD_hpp

#include <wepa/adt/RuntimeException.hpp>

#include <wepa/xml/Wrapper.hpp>

struct _xmlDtd;
struct _xmlValidCtxt;

namespace boost {
   namespace filesystem {
      class path;
   }
}

namespace wepa {

namespace adt {
   class DataBlock;
}

namespace xml {

class Document;

class DTD : public Wrapper <_xmlDtd> {
public:
   DTD ();
   virtual ~DTD ();

   void initialize (const boost::filesystem::path& path) throw (adt::RuntimeException);
   void initialize (const char* buffer, const size_t size) throw (adt::RuntimeException);
   void initialize (const adt::DataBlock& buffer) throw (adt::RuntimeException);

private:
   static char st_text [1024];
   _xmlValidCtxt* m_context;

   void validate (const xml::Document* document) const throw (adt::RuntimeException);

   static const char* nameExtractor(const Handler handler) noexcept;

   static void callbackErrorHandler (void *ctx,  const char *msg, ...) noexcept;
   static void callbackWarningHandler (void *ctx,  const char *msg, ...) noexcept;

   friend class wepa::xml::Document;
};

} /* namespace xml */
} /* namespace wepa */
#endif
