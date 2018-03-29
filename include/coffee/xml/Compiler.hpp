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

#ifndef __coffee_xml_Compiler_hpp
#define __coffee_xml_Compiler_hpp

#include <memory>

#include <coffee/basis/RuntimeException.hpp>
#include <coffee/basis/DataBlock.hpp>

#include <coffee/xml/Wrapper.hpp>

struct _xmlTextWriter;
struct _xmlBuffer;

namespace coffee {
namespace xml {

class Document;
class Node;

class Compiler : public Wrapper <_xmlTextWriter> {
public:
   Compiler ();
   virtual ~Compiler ();

   std::string apply (const Document& document) throw (basis::RuntimeException);
   std::string apply (const std::shared_ptr<Node>& node) throw (basis::RuntimeException);

   const char* encode (const char* text) throw (basis::RuntimeException);
   const char* encode (const std::string& text) throw (basis::RuntimeException) { return encode (text.c_str ()); }

   void setEncoding (const char* encoding) noexcept { m_encoding = encoding; }

private:
   const char* m_encoding;
   char* m_encodedBuffer;
   int  m_encodedReservedSize;

   Compiler (const Compiler&);

   class Buffer : public Wrapper <_xmlBuffer> {
   public:
      Buffer ();
      ~Buffer () {;}
      const char* getValue () const noexcept;
   };
};

} /* namespace xml */
} /* namespace coffee */
#endif
