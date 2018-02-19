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
#ifndef __coffee_xxxx_Attribute_hpp
#define __coffee_xxxx_Attribute_hpp

#include <coffee/adt/AsString.hpp>
#include <coffee/adt/RuntimeException.hpp>

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

   void setValue (const char* value) throw (adt::RuntimeException);
   void setValue (const std::string& value) throw (adt::RuntimeException) { setValue (value.c_str ()); }

   template <typename _T> void setValue(const _T value) throw (adt::RuntimeException) {
      setValue (adt::AsString::apply(value));
   }
   const std::string& getValue () const noexcept;

   bool operator < (const Attribute& left) const noexcept;

private:
   mutable Content m_value;

   static const char* nameExtractor (const Handler handler) noexcept;

   void compile (Compiler& compiler) const throw (adt::RuntimeException);

   friend class Document;
   friend class Node;
};

} /* namespace xml */
} /* namespace coffee */
#endif
