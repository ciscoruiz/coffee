#ifndef __coffee_xml_Content_hpp
#define __coffee_xml_Content_hpp

#include <string>
#include <memory>

namespace coffee {
namespace xml {

class Content {
public:
   Content () {;}
   virtual ~Content () { clear (); }

   bool isNull () const noexcept { return m_value.get () == NULL; }
   const std::string& getValue () const noexcept { return std::ref (*m_value.get ()); }
   void setValue (const char* text) noexcept;
   void clear () { m_value.reset (NULL); }

private:
   std::unique_ptr <std::string> m_value;
};

} /* namespace xml */
} /* namespace coffee */
#endif
