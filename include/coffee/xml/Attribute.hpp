#ifndef __coffee_xml_Attribute_hpp
#define __coffee_xml_Attribute_hpp

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
