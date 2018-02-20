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

   explicit Node(const char* name);
   explicit Node(_xmlNode* xmlNode);
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
