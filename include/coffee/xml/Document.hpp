#ifndef __coffee_xml_Document_hpp
#define __coffee_xml_Document_hpp

struct _xmlDoc;
struct _xmlValidCtxt;

namespace boost {
   namespace filesystem {
      class path;
   }
}

#include <memory>
#include <coffee/adt/RuntimeException.hpp>

#include <coffee/xml/Wrapper.hpp>

namespace coffee {

namespace adt {
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

   void parse(const boost::filesystem::path& path) throw(adt::RuntimeException);
   void parse(const adt::DataBlock& buffer) throw(adt::RuntimeException);
   void parse(const char* buffer, const size_t size) throw(adt::RuntimeException);

   void parse(const boost::filesystem::path& path, const DTD& dtd) throw(adt::RuntimeException);
   void parse(const adt::DataBlock& buffer, const DTD& dtd) throw(adt::RuntimeException);
   void parse(const char* buffer, const size_t size, const DTD& dtd) throw(adt::RuntimeException);

   const std::shared_ptr<Node> getRoot() const noexcept { return m_root; }
   std::shared_ptr<Node> getRoot() noexcept { return m_root; }

private:
   std::shared_ptr <Node> m_root;

   void parseFile(const boost::filesystem::path& path) throw(adt::RuntimeException);
   void parseMemory(const adt::DataBlock& buffer) throw(adt::RuntimeException);

   void extractNodes(_xmlDoc* handler) throw(adt::RuntimeException);

   static const char* nameExtractor(const Handler handler) noexcept;

   static void extractNodes(std::shared_ptr<xml::Node>& node) throw(adt::RuntimeException);
   static void extractAttributes(std::shared_ptr<xml::Node>& node) throw(adt::RuntimeException);

   void compile(Compiler& compiler) const throw(adt::RuntimeException);

   friend class Compiler;
};

} /* namespace xml */
} /* namespace coffee */
#endif
