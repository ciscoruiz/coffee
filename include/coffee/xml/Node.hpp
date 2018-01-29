// COFFEE - COmpany eFFEEctive Platform
//
//(c) Copyright 2018 Francisco Ruiz Rayo
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#ifndef __coffee_xml_Node_hpp
#define __coffee_xml_Node_hpp

#include <memory>
#include <vector>
#include <set>

#include <coffee/adt/RuntimeException.hpp>
#include <coffee/adt/AsString.hpp>

#include <coffee/xml/Wrapper.hpp>
#include <coffee/xml/Attribute.hpp>
#include <coffee/xml/Content.hpp>

struct _xmlNode;
struct _xmlAttr;

namespace coffee {
namespace xml {

class Document;
class Attribute;
class Compiler;

class Node : public Wrapper <_xmlNode> {
   struct LessAttributePtr {
      bool operator()(const std::shared_ptr<Attribute>& lhs, const std::shared_ptr<Attribute>& rhs) const {
         return lhs->getName() < rhs->getName();
      }
   };

   typedef std::vector<std::shared_ptr<Node> > ChildrenContainer;
   typedef std::set<std::shared_ptr<Attribute>, LessAttributePtr> AttributeContainer;

public:
   typedef ChildrenContainer::iterator child_iterator;
   typedef ChildrenContainer::const_iterator const_child_iterator;

   typedef AttributeContainer::const_iterator const_attribute_iterator;

   Node(const char* name);
   Node(_xmlNode* xmlNode);
   ~Node() {;}

   std::shared_ptr<Node> lookupChild(const char* name) const throw(adt::RuntimeException);
   std::shared_ptr<Node> searchChild(const char* name) const noexcept;
   std::shared_ptr<Node> lookupChild(const std::string& name) const throw(adt::RuntimeException) { return lookupChild(name.c_str()); }
   std::shared_ptr<Node> searchChild(const std::string& name) const noexcept { return searchChild(name.c_str()); }
   std::shared_ptr<Node> childAt(const size_t ii) const throw(adt::RuntimeException);

   std::shared_ptr<Node> lookupChild(const char* name) throw(adt::RuntimeException);
   std::shared_ptr<Node> searchChild(const char* name) noexcept;
   std::shared_ptr<Node> lookupChild(const std::string& name) throw(adt::RuntimeException) { return lookupChild(name.c_str()); }
   std::shared_ptr<Node> searchChild(const std::string& name) noexcept { return searchChild(name.c_str()); }
   std::shared_ptr<Node> childAt(const size_t ii) throw(adt::RuntimeException);

   std::shared_ptr<Node> createChild(const char* name) throw(adt::RuntimeException);
   std::shared_ptr<Node> createChild(const std::string& name) throw(adt::RuntimeException) { return createChild(name.c_str()); }
   std::shared_ptr<Attribute> createAttribute(const char* name, const char* value) throw(adt::RuntimeException);
   std::shared_ptr<Attribute> createAttribute(const char* name, const std::string& value) throw(adt::RuntimeException) { return createAttribute(name, value.c_str()); }

   template <typename _T> std::shared_ptr<Attribute> createAttribute(const char* name, const _T value) throw(adt::RuntimeException) {
      return createAttribute(name, adt::AsString::apply(value));
   }

   void createText(const char* text) throw(adt::RuntimeException);
   void createText(const std::string& text) throw(adt::RuntimeException) { createText(text.c_str()); }

   std::shared_ptr<Attribute> lookupAttribute(const char* name) const throw(adt::RuntimeException);
   std::shared_ptr<Attribute> searchAttribute(const char* name) const noexcept;
   std::shared_ptr<Attribute> lookupAttribute(const std::string& name) const throw(adt::RuntimeException) { return lookupAttribute(name.c_str()); }
   std::shared_ptr<Attribute> searchAttribute(const std::string& name) const noexcept { return searchAttribute(name.c_str()); }

   child_iterator child_begin() noexcept { return m_children.begin(); }
   child_iterator child_end() noexcept { return m_children.end(); }
   static std::shared_ptr<Node>& get_child(child_iterator ii) noexcept { return std::ref(*ii); }

   size_t children_size() const noexcept { return m_children.size(); }
   const_child_iterator child_begin() const noexcept { return m_children.begin(); }
   const_child_iterator child_end() const noexcept { return m_children.end(); }
   static const std::shared_ptr<Node>& get_child(const_child_iterator ii) noexcept { return std::ref(*ii); }

   size_t attributes_size() const noexcept { return m_attributes.size(); }
   const_attribute_iterator attribute_begin() const noexcept { return m_attributes.begin(); }
   const_attribute_iterator attribute_end() const noexcept { return m_attributes.end(); }
   static const std::shared_ptr<Attribute>& get_attribute(const_attribute_iterator ii) noexcept { return std::ref(*ii); }

   bool hasText() const noexcept { return m_text.isNull() == false; }
   const std::string& getText() const throw(adt::RuntimeException);
   void setText(const char* text) noexcept { m_text.setValue(text); }

private:
   ChildrenContainer m_children;
   AttributeContainer m_attributes;
   Content m_text;

   void addChild(std::shared_ptr<Node> child) noexcept { m_children.push_back(child); }
   void addAttribute(std::shared_ptr<Attribute> attribute) throw(adt::RuntimeException);

   static const char* nameExtractor(const Handler handler) noexcept;

   void compile(Compiler& compiler) const throw(adt::RuntimeException);

   friend class Compiler;
   friend class Document;
};

} /* namespace xml */
} /* namespace coffee */

#endif
