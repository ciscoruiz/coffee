// COFFEE - COmpany eFFEEctive Platform
//
// (c) Copyright 2018 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/coffee
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
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
