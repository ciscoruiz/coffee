#ifndef __coffee_xml_DTD_hpp
#define __coffee_xml_DTD_hpp

#include <coffee/adt/RuntimeException.hpp>

#include <coffee/xml/Wrapper.hpp>

struct _xmlDtd;
struct _xmlValidCtxt;

namespace boost {
   namespace filesystem {
      class path;
   }
}

namespace coffee {

namespace adt {
   class DataBlock;
}

namespace xml {

class Document;

class DTD : public Wrapper <_xmlDtd> {
public:
   DTD ();
   virtual ~DTD ();

   void initialize (const boost::filesystem::path& path) throw (adt::RuntimeException);
   void initialize (const char* buffer, const size_t size) throw (adt::RuntimeException);
   void initialize (const adt::DataBlock& buffer) throw (adt::RuntimeException);

private:
   static char st_text [1024];
   _xmlValidCtxt* m_context;

   void validate (const xml::Document* document) const throw (adt::RuntimeException);

   static const char* nameExtractor(const Handler handler) noexcept;

   static void callbackErrorHandler (void *ctx,  const char *msg, ...) noexcept;
   static void callbackWarningHandler (void *ctx,  const char *msg, ...) noexcept;

   friend class coffee::xml::Document;
};

} /* namespace xml */
} /* namespace coffee */
#endif
