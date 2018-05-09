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

#include <libxml/xmlstring.h>
#include <libxml/tree.h>

#include <coffee/xml/Content.hpp>

using namespace coffee;

// static
const std::string xml::Content::WithoutName = "no-name";

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
