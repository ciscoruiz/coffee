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
#include <libxml/parser.h>
#include <libxml/xmlwriter.h>

#include <iostream>
#include <boost/filesystem.hpp>

#include <wepa/adt/DataBlock.hpp>
#include <wepa/adt/AsString.hpp>

#include <wepa/xml/Document.hpp>
#include <wepa/xml/DTD.hpp>
#include <wepa/xml/Node.hpp>
#include <wepa/xml/SCCS.hpp>
#include <wepa/xml/Compiler.hpp>

using namespace wepa;

xml::Document::Document () :
   Wrapper ()
{
   xml::SCCS::activate ();

   setDeleter(xmlFreeDoc);
   setNameExtractor(nameExtractor);
}

//virtual
xml::Document::~Document ()
{
}

// static
const char* xml::Document::nameExtractor (const Handler handler)
   noexcept
{
   return (const char*) (handler->name);
}

void xml::Document::parse (const boost::filesystem::path& file)
   throw (adt::RuntimeException)
{
   releaseHandler();
   parseFile (file);
   extractNodes (getHandler ());

   xmlCleanupParser();
}

void xml::Document::parse (const adt::DataBlock& buffer)
   throw (adt::RuntimeException)
{
   releaseHandler();
   parseMemory (buffer);
   extractNodes (getHandler ());

   xmlCleanupParser();
}

void xml::Document::parse (const char* buffer, const size_t size)
   throw (adt::RuntimeException)
{
   const adt::DataBlock data (buffer, size);
   parse (data);
}

void xml::Document::parse (const boost::filesystem::path& file, const DTD& dtd)
   throw (adt::RuntimeException)
{
   releaseHandler();
   parseFile (file);
   dtd.validate(this);
   extractNodes (getHandler ());
}

void xml::Document::parse (const adt::DataBlock& buffer, const DTD& dtd)
   throw (adt::RuntimeException)
{
   releaseHandler();
   parseMemory (buffer);
   dtd.validate(this);
   extractNodes (getHandler ());
}

void xml::Document::parse (const char* buffer, const size_t size, const DTD& dtd)
   throw (adt::RuntimeException)
{
   const adt::DataBlock data (buffer, size);
   parse (data, dtd);
}

void xml::Document::parseFile (const boost::filesystem::path& file)
   throw (adt::RuntimeException)
{
   if (boost::filesystem::exists (file) == false) {
      WEPA_THROW_EXCEPTION("File '" << file.c_str () << "' does not exist");
   }

   if (setHandler (xmlParseFile (file.c_str ())) == NULL)
      WEPA_THROW_EXCEPTION("Some errors were found while parsing filename '" << file.c_str () << "'");
}

void xml::Document::parseMemory (const adt::DataBlock& buffer)
   throw (adt::RuntimeException)
{
   if (setHandler (xmlParseMemory(buffer.data (), buffer.size ())) == NULL)
      WEPA_THROW_EXCEPTION("Some errors were found while parsing memory: " << adt::AsString::apply (buffer));
}

void xml::Document::extractNodes (_xmlDoc* handler)
   throw (adt::RuntimeException)
{
   xmlNodePtr root = xmlDocGetRootElement(handler);

   xml::Node* node = new xml::Node (root);

   m_root.reset (node);

   extractNodes(*m_root);
   extractAttributes (*m_root);
}

// static
void xml::Document::extractNodes (xml::Node& node)
   throw (adt::RuntimeException)
{
   xml::Node::Handler handler = node.getHandler()->children;
   xml::Node* child;

   while (handler != NULL) {
      switch (handler->type) {
      case XML_ELEMENT_NODE:
         child = new xml::Node (handler);
         node.addChild(child);
         extractNodes(*child);
         extractAttributes(*child);
         break;
      case XML_TEXT_NODE:
         node.setText ((const char*) (handler->content));
         break;
      }

      handler = handler->next;
   }
}

//static
void xml::Document::extractAttributes (xml::Node& node)
   throw (adt::RuntimeException)
{
   xmlAttrPtr xmlAttribute = node.getHandler()->properties;

   while (xmlAttribute != NULL) {
      node.addAttribute(new xml::Attribute (xmlAttribute));
      xmlAttribute = xmlAttribute->next;
   }
}

void xml::Document::compile (xml::Compiler& compiler) const
   throw (adt::RuntimeException)
{
   Compiler::Handler compilerHandler = compiler.getHandler();

   Handler handler = getHandler();

   if (xmlTextWriterStartDocument(compiler, (const char*) handler->version, (const char*) handler->encoding, NULL) < 0)
      WEPA_THROW_EXCEPTION("Document " << getName() << " could not start document");

   if (m_root.get() != NULL)
      m_root.get ()->compile (compiler);

   if (xmlTextWriterEndDocument(compiler) < 0)
      WEPA_THROW_EXCEPTION("Document " << getName() << " could not start document");
}

