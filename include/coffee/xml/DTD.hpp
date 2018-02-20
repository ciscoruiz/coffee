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

#ifndef __coffee_xml_DTD_hpp
#define __coffee_xml_DTD_hpp

#include <coffee/adt/RuntimeException.hpp>

#include <coffee/xml/Wrapper.hpp>

struct _xmlDtd;
struct _xmlValidCtxt;

namespace boost {
   namespace filesystem {
      class path;
   }
}

namespace coffee {

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

   friend class coffee::xml::Document;
};

} /* namespace xml */
} /* namespace coffee */
#endif
