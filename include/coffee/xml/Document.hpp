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

#ifndef __coffee_xml_Document_hpp
#define __coffee_xml_Document_hpp

struct _xmlDoc;
struct _xmlError;

namespace boost {
   namespace filesystem {
      class path;
   }
}

#include <memory>
#include <coffee/basis/RuntimeException.hpp>

#include <coffee/xml/Wrapper.hpp>

namespace coffee {

namespace basis {
   class DataBlock;
}

namespace xml {

class Node;
class DTD;
class Compiler;

class Document : public Wrapper <_xmlDoc> {
public:
   Document();
   virtual ~Document();

   const Document& parse(const boost::filesystem::path& path) throw(basis::RuntimeException);
   const Document& parse(const basis::DataBlock& buffer) throw(basis::RuntimeException);

   const Document& parse(const boost::filesystem::path& path, const DTD& dtd) throw(basis::RuntimeException);
   const Document& parse(const basis::DataBlock& buffer, const DTD& dtd) throw(basis::RuntimeException);

   const std::shared_ptr<Node> getRoot() const noexcept { return m_root; }
   std::shared_ptr<Node> getRoot() noexcept { return m_root; }

private:
   std::shared_ptr <Node> m_root;

   void parseFile(const boost::filesystem::path& path) throw(basis::RuntimeException);
   void parseMemory(const basis::DataBlock& buffer) throw(basis::RuntimeException);

   void extractNodes(_xmlDoc* handler) throw(basis::RuntimeException);

   const char* readName(const Handler handler) const noexcept;

   static void extractNodes(std::shared_ptr<xml::Node>& node) throw(basis::RuntimeException);
   static void extractAttributes(std::shared_ptr<xml::Node>& node) throw(basis::RuntimeException);

   void compile(Compiler& compiler) const throw(basis::RuntimeException);

   static void logError(void* data, _xmlError* error) noexcept;

   friend class Compiler;
};

} /* namespace xml */
} /* namespace coffee */
#endif
