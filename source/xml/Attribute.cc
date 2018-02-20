#include <string.h>

#include <libxml/tree.h>
#include <libxml/xmlwriter.h>

#include <coffee/xml/Attribute.hpp>
#include <coffee/xml/Compiler.hpp>

using namespace coffee;

xml::Attribute::Attribute (_xmlAttr* handler) : Wrapper (handler)
{
   setNameExtractor(nameExtractor);
}

const char* xml::Attribute::nameExtractor (const Handler handler)
   noexcept
{
   return (const char*) (handler->name);
}

void xml::Attribute::setValue (const char* value)
   throw (adt::RuntimeException)
{
   Handler handler = getHandler();
   xmlSetProp(handler->parent, BAD_CAST handler->name, BAD_CAST value);
   m_value.clear ();
}

const std::string& xml::Attribute::getValue () const
   noexcept
{
   Handler handler = getHandler();

   if (m_value.isNull () == true) {
      const char* value = (handler->children != NULL) ? (const char*) handler->children->content: "";
      m_value.setValue(value);
   }

   return m_value.getValue();
}

bool xml::Attribute::operator < (const Attribute& left) const
   noexcept
{
   const char* thisName = nameExtractor (getHandler());
   const char* leftName = nameExtractor (left.getHandler());

   if (thisName == NULL && leftName == NULL)
      return false;

   if (thisName == NULL && leftName != NULL)
      return true;

   if (thisName != NULL && leftName == NULL)
      return false;

   return strcmp (thisName, leftName) < 0;
}

void xml::Attribute::compile (Compiler& compiler) const
   throw (adt::RuntimeException)
{
   xmlNsPtr nameSpace = getHandler()->ns;

   int rc;

   const unsigned char* name = BAD_CAST (getName ().c_str ());
   const unsigned char* value = BAD_CAST (compiler.encode (getValue ()));

   if (nameSpace == NULL)
      rc = xmlTextWriterWriteAttribute (compiler, name, value);
   else
      rc = xmlTextWriterWriteAttributeNS (compiler, BAD_CAST (nameSpace->prefix), name, BAD_CAST (nameSpace->href), value);

   if (rc < 0)
      COFFEE_THROW_EXCEPTION("Can not compile attribute " << getName ());
}
