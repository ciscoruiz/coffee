#include <libxml/parser.h>
#include <libxml/xmlwriter.h>

#include <iostream>
#include <boost/filesystem.hpp>
#include <memory>

#include <coffee/adt/DataBlock.hpp>
#include <coffee/adt/AsString.hpp>

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
   setNameExtractor(nameExtractor);
}

//virtual
xml::Document::~Document()
{
}

// static
const char* xml::Document::nameExtractor(const Handler handler)
   noexcept
{
   return(const char*)(handler->name);
}

void xml::Document::parse(const boost::filesystem::path& file)
   throw(adt::RuntimeException)
{
   releaseHandler();
   parseFile(file);
   extractNodes(getHandler());

   xmlCleanupParser();
}

void xml::Document::parse(const adt::DataBlock& buffer)
   throw(adt::RuntimeException)
{
   releaseHandler();
   parseMemory(buffer);
   extractNodes(getHandler());

   xmlCleanupParser();
}

void xml::Document::parse(const char* buffer, const size_t size)
   throw(adt::RuntimeException)
{
   const adt::DataBlock data(buffer, size);
   parse(data);
}

void xml::Document::parse(const boost::filesystem::path& file, const DTD& dtd)
   throw(adt::RuntimeException)
{
   releaseHandler();
   parseFile(file);
   dtd.validate(this);
   extractNodes(getHandler());
}

void xml::Document::parse(const adt::DataBlock& buffer, const DTD& dtd)
   throw(adt::RuntimeException)
{
   releaseHandler();
   parseMemory(buffer);
   dtd.validate(this);
   extractNodes(getHandler());
}

void xml::Document::parse(const char* buffer, const size_t size, const DTD& dtd)
   throw(adt::RuntimeException)
{
   const adt::DataBlock data(buffer, size);
   parse(data, dtd);
}

void xml::Document::parseFile(const boost::filesystem::path& file)
   throw(adt::RuntimeException)
{
   if(boost::filesystem::exists(file) == false) {
      COFFEE_THROW_EXCEPTION("File '" << file.c_str() << "' does not exist");
   }

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

