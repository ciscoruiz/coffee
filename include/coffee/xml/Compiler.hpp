#ifndef __coffee_xml_Compiler_hpp
#define __coffee_xml_Compiler_hpp

#include <memory>

#include <coffee/adt/RuntimeException.hpp>
#include <coffee/adt/DataBlock.hpp>

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

   std::string apply (const Document& document) throw (adt::RuntimeException);
   std::string apply (const std::shared_ptr<Node>& node) throw (adt::RuntimeException);

   const char* encode (const char* text) throw (adt::RuntimeException);
   const char* encode (const std::string& text) throw (adt::RuntimeException) { return encode (text.c_str ()); }

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
