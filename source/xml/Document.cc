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

#include <libxml/parser.h>
#include <libxml/xmlwriter.h>

#include <iostream>
#include <boost/filesystem.hpp>
#include <memory>

#include <coffee/adt/DataBlock.hpp>
#include <coffee/adt/AsString.hpp>

#include <coffee/logger/Logger.hpp>

#include <coffee/xml/Document.hpp>
#include <coffee/xml/DTD.hpp>
#include <coffee/xml/Node.hpp>
#include <coffee/xml/SCCS.hpp>
#include <coffee/xml/Compiler.hpp>

using namespace coffee;

xml::Document::Document() :
   Wrapper()
{
   xml::SCCS::activate();
   setDeleter(xmlFreeDoc);
}

//virtual
xml::Document::~Document()
{
}

const char* xml::Document::readName(const Handler handler) const
   noexcept
{
   return (handler->URL != NULL) ? (const char*)(handler->URL): Content::WithoutName.c_str();
}

const xml::Document& xml::Document::parse(const boost::filesystem::path& file)
   throw(adt::RuntimeException)
{
   releaseHandler();
   parseFile(file);
   extractNodes(getHandler());
   xmlCleanupParser();
   return *this;
}

const xml::Document& xml::Document::parse(const adt::DataBlock& buffer)
   throw(adt::RuntimeException)
{
   releaseHandler();
   parseMemory(buffer);
   extractNodes(getHandler());
   xmlCleanupParser();
   return *this;
}

const xml::Document& xml::Document::parse(const boost::filesystem::path& file, const DTD& dtd)
   throw(adt::RuntimeException)
{
   releaseHandler();
   parseFile(file);
   dtd.validate(*this);
   extractNodes(getHandler());
   xmlCleanupParser();
   return *this;
}

const xml::Document& xml::Document::parse(const adt::DataBlock& buffer, const DTD& dtd)
   throw(adt::RuntimeException)
{
   releaseHandler();
   parseMemory(buffer);
   dtd.validate(*this);
   extractNodes(getHandler());
   xmlCleanupParser();
   return *this;
}

void xml::Document::parseFile(const boost::filesystem::path& file)
   throw(adt::RuntimeException)
{
   if(boost::filesystem::exists(file) == false) {
      COFFEE_THROW_EXCEPTION("File '" << file.c_str() << "' does not exist");
   }

   LOG_DEBUG("Parsing XML " << file.c_str());

   if(setHandler(xmlParseFile(file.c_str())) == NULL)
      COFFEE_THROW_EXCEPTION("Some errors were found while parsing filename '" << file.c_str() << "'");
}

void xml::Document::parseMemory(const adt::DataBlock& buffer)
   throw(adt::RuntimeException)
{
   if(setHandler(xmlParseMemory(buffer.data(), buffer.size())) == NULL)
      COFFEE_THROW_EXCEPTION("Some errors were found while parsing memory: " << adt::AsString::apply(buffer));
}

void xml::Document::extractNodes(_xmlDoc* handler)
   throw(adt::RuntimeException)
{
   xmlNodePtr root = xmlDocGetRootElement(handler);

   m_root = std::make_shared<xml::Node>(root);
   extractNodes(m_root);
   extractAttributes(m_root);
}

// static
void xml::Document::extractNodes(std::shared_ptr<xml::Node>& node)
   throw(adt::RuntimeException)
{
   xml::Node::Handler handler = node->getHandler()->children;
   std::shared_ptr<xml::Node> child;

   while(handler != NULL) {
      switch(handler->type) {
      case XML_ELEMENT_NODE:
         child = std::make_shared<xml::Node>(handler);
         node->addChild(child);
         extractNodes(child);
         extractAttributes(child);
         break;
      case XML_TEXT_NODE:
         node->setText((const char*)(handler->content));
         break;
      }

      handler = handler->next;
   }
}

//static
void xml::Document::extractAttributes(std::shared_ptr<xml::Node>& node)
   throw(adt::RuntimeException)
{
   xmlAttrPtr xmlAttribute = node->getHandler()->properties;

   while(xmlAttribute != NULL) {
      node->addAttribute(std::make_shared<xml::Attribute>(xmlAttribute));
      xmlAttribute = xmlAttribute->next;
   }
}

void xml::Document::compile(xml::Compiler& compiler) const
   throw(adt::RuntimeException)
{
   Compiler::Handler compilerHandler = compiler.getHandler();

   Handler handler = getHandler();

   if(xmlTextWriterStartDocument(compiler,(const char*) handler->version,(const char*) handler->encoding, NULL) < 0)
      COFFEE_THROW_EXCEPTION("Document " << getName() << " could not start document");

   if(m_root)
      m_root->compile(compiler);

   if(xmlTextWriterEndDocument(compiler) < 0)
      COFFEE_THROW_EXCEPTION("Document " << getName() << " could not start document");
}

