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

#ifndef __coffee_xml_Node_hpp
#define __coffee_xml_Node_hpp

#include <memory>
#include <vector>
#include <set>

#include <coffee/basis/RuntimeException.hpp>
#include <coffee/basis/AsString.hpp>

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

   explicit Node(const char* name);
   explicit Node(_xmlNode* xmlNode);
   ~Node() {;}

   std::shared_ptr<Node> lookupChild(const char* name) const throw(basis::RuntimeException);
   std::shared_ptr<Node> searchChild(const char* name) const noexcept;
   std::shared_ptr<Node> lookupChild(const std::string& name) const throw(basis::RuntimeException) { return lookupChild(name.c_str()); }
   std::shared_ptr<Node> searchChild(const std::string& name) const noexcept { return searchChild(name.c_str()); }
   std::shared_ptr<Node> childAt(const size_t ii) const throw(basis::RuntimeException);

   std::shared_ptr<Node> lookupChild(const char* name) throw(basis::RuntimeException);
   std::shared_ptr<Node> searchChild(const char* name) noexcept;
   std::shared_ptr<Node> lookupChild(const std::string& name) throw(basis::RuntimeException) { return lookupChild(name.c_str()); }
   std::shared_ptr<Node> searchChild(const std::string& name) noexcept { return searchChild(name.c_str()); }
   std::shared_ptr<Node> childAt(const size_t ii) throw(basis::RuntimeException);

   std::shared_ptr<Node> createChild(const char* name) throw(basis::RuntimeException);
   std::shared_ptr<Node> createChild(const std::string& name) throw(basis::RuntimeException) { return createChild(name.c_str()); }
   std::shared_ptr<Attribute> createAttribute(const char* name, const char* value) throw(basis::RuntimeException);
   std::shared_ptr<Attribute> createAttribute(const char* name, const std::string& value) throw(basis::RuntimeException) { return createAttribute(name, value.c_str()); }

   template <typename _T> std::shared_ptr<Attribute> createAttribute(const char* name, const _T value) throw(basis::RuntimeException) {
      return createAttribute(name, basis::AsString::apply(value));
   }

   void createText(const char* text) throw(basis::RuntimeException);
   void createText(const std::string& text) throw(basis::RuntimeException) { createText(text.c_str()); }

   std::shared_ptr<Attribute> lookupAttribute(const char* name) const throw(basis::RuntimeException);
   std::shared_ptr<Attribute> searchAttribute(const char* name) const noexcept;
   std::shared_ptr<Attribute> lookupAttribute(const std::string& name) const throw(basis::RuntimeException) { return lookupAttribute(name.c_str()); }
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
   const std::string& getText() const throw(basis::RuntimeException);
   void setText(const char* text) noexcept { m_text.setValue(text); }

private:
   ChildrenContainer m_children;
   AttributeContainer m_attributes;
   Content m_text;

   void addChild(std::shared_ptr<Node> child) noexcept { m_children.push_back(child); }
   void addAttribute(std::shared_ptr<Attribute> attribute) throw(basis::RuntimeException);

   const char* readName(const Handler handler) const noexcept;

   void compile(Compiler& compiler) const throw(basis::RuntimeException);

   friend class Compiler;
   friend class Document;
};

} /* namespace xml */
} /* namespace coffee */

#endif
