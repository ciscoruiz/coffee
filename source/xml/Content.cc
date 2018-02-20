#include <libxml/xmlstring.h>
#include <libxml/tree.h>

#include <coffee/xml/Content.hpp>

using namespace coffee;

void xml::Content::setValue (const char* value)
   noexcept
{
   unsigned char* utf8Text = NULL;
   const unsigned char* source = reinterpret_cast <const unsigned char*> (value);
   bool releaseMemory = false;

   if (xmlCheckUTF8 (source) == 0) {
      utf8Text = const_cast <unsigned char*> (source);
      releaseMemory = false;
   }
   else {
      int srcLen = xmlStrlen(source);
      int targetLen = srcLen;
      utf8Text = new unsigned char [srcLen + 1];
      UTF8Toisolat1 (utf8Text, &targetLen, source, &srcLen);
      utf8Text [targetLen] = 0;
      releaseMemory = true;
   }

   if (m_value.get () != NULL) {
      *m_value = (const char*) utf8Text;
   }
   else {
      std::string* ww = new std::string ((const char*) utf8Text);
      m_value.reset (ww);
   }

   if (releaseMemory == true)
      delete [] utf8Text;
}
