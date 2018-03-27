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

#ifndef __coffee_xml_Attribute_hpp
#define __coffee_xml_Attribute_hpp

#include <coffee/basis/AsString.hpp>
#include <coffee/basis/RuntimeException.hpp>

#include <coffee/xml/Wrapper.hpp>
#include <coffee/xml/Content.hpp>

struct _xmlAttr;

namespace coffee {
namespace xml {

class Document;
class Compile;

class Attribute : public Wrapper <_xmlAttr> {
public:
   explicit Attribute (_xmlAttr* handler);
   virtual ~Attribute () {;}

   void setValue (const char* value) throw (basis::RuntimeException);
   void setValue (const std::string& value) throw (basis::RuntimeException) { setValue (value.c_str ()); }

   template <typename _T> void setValue(const _T value) throw (basis::RuntimeException) {
      setValue (basis::AsString::apply(value));
   }
   const std::string& getValue () const noexcept;

   bool operator < (const Attribute& left) const noexcept;

private:
   mutable Content m_value;

   const char* readName(const Handler handler) const noexcept;

   void compile (Compiler& compiler) const throw (basis::RuntimeException);

   friend class Document;
   friend class Node;
};

} /* namespace xml */
} /* namespace coffee */
#endif
