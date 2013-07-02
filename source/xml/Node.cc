// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2013 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/wepa
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
#include <libxml/tree.h>
//#include <libxml/xmlstring.h>

#include <wepa/xml/Node.hpp>

using namespace wepa;

xml::Node::Node (_xmlNode* handler) : Wrapper (handler)
{
   setNameExtractor(nameExtractor);
}

// static
const char* xml::Node::nameExtractor (const Handler handler)
   throw ()
{
   return (const char*) (handler->name);
}

const std::string& xml::Node::getText () const
   throw (adt::RuntimeException)
{
   if (hasText() == false) {
      WEPA_THROW_EXCEPTION ("Node '" << getName () << "' does not have any TEXT item");
   }

   return m_text.getValue ();
}

void xml::Node::addAttribute (Attribute* attribute)
   throw (adt::RuntimeException)
{
   auto rr = m_attributes.insert(attribute);

   if (rr.second ==  false)
      WEPA_THROW_EXCEPTION(attribute->getName () << " is already defined in node '" << getName () << "'");
}

const xml::Node& xml::Node::lookupChild (const char* name) const
   throw (adt::RuntimeException)
{
   const Node* result = NULL;

   if ((result = searchChild (name)) == NULL) {
      WEPA_THROW_EXCEPTION("Child '" << name << "' is not registered in node '" << getName () << "'");
   }

   return std::ref (*result);
}

const xml::Node* xml::Node::searchChild (const char* name) const
   throw ()
{
   for (const_child_iterator ii = child_begin(), maxii = child_end (); ii != maxii; ++ ii) {
      const Node& child = get_child(ii);

      if (child.getName ()  == name)
         return &child;
   }

   return NULL;
}

const xml::Node& xml::Node::childAt (const size_t ii) const
   throw (adt::RuntimeException)
{
   if (ii >= children_size()) {
      WEPA_THROW_EXCEPTION("There is not any child at position: " << ii);
   }

   return get_child (child_begin () + ii);
}

xml::Node& xml::Node::lookupChild (const char* name)
   throw (adt::RuntimeException)
{
   Node* result = NULL;

   if ((result = searchChild (name)) == NULL) {
      WEPA_THROW_EXCEPTION("Child '" << name << "' is not registered in node '" << getName () << "'");
   }

   return std::ref (*result);
}

xml::Node* xml::Node::searchChild (const char* name)
   throw ()
{
   for (child_iterator ii = child_begin(), maxii = child_end (); ii != maxii; ++ ii) {
      Node& child = get_child(ii);

      if (child.getName ()  == name)
         return &child;
   }

   return NULL;
}

xml::Node& xml::Node::childAt (const size_t ii)
   throw (adt::RuntimeException)
{
   if (ii >= children_size()) {
      WEPA_THROW_EXCEPTION("There is not any child at position: " << ii);
   }

   return get_child (child_begin () + ii);
}

const xml::Attribute& xml::Node::lookupAttribute (const char* name) const
   throw (adt::RuntimeException)
{
   const Attribute* result = NULL;

   if ((result = searchAttribute (name)) == NULL) {
      WEPA_THROW_EXCEPTION("Attribute '" << name << "' is not registered in node '" << getName () << "'");
   }

   return std::ref (*result);
}

const xml::Attribute* xml::Node::searchAttribute (const char* name) const
   throw ()
{
   for (const_attribute_iterator ii = attribute_begin(), maxii = attribute_end (); ii != maxii; ++ ii) {
      const Attribute& attribute = get_attribute(ii);

      if (attribute.getName ()  == name)
         return &attribute;
   }

   return NULL;
}

xml::Attribute& xml::Node::lookupAttribute (const char* name)
   throw (adt::RuntimeException)
{
   Attribute* result = NULL;

   if ((result = searchAttribute (name)) == NULL) {
      WEPA_THROW_EXCEPTION("Attribute '" << name << "' is not registered in node '" << getName () << "'");
   }

   return std::ref (*result);
}

xml::Attribute* xml::Node::searchAttribute (const char* name)
   throw ()
{
   for (attribute_iterator ii = attribute_begin(), maxii = attribute_end (); ii != maxii; ++ ii) {
      Attribute& attribute = get_attribute(ii);

      if (attribute.getName ()  == name)
         return &attribute;
   }

   return NULL;
}


