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

#include <libxml/xmlwriter.h>
#include <libxml/encoding.h>

#include <coffee/xml/Compiler.hpp>
#include <coffee/xml/Document.hpp>
#include <coffee/xml/Node.hpp>

using namespace coffee;

xml::Compiler::Compiler() :
   m_encoding(NULL),
   m_encodedBuffer(NULL),
   m_encodedReservedSize(0)
{
   setHandler(NULL);
   setDeleter(xmlFreeTextWriter);
}

xml::Compiler::~Compiler()
{
   if(m_encodedBuffer)
      xmlFree(m_encodedBuffer);
}

std::string xml::Compiler::apply(const Document& document)
   throw(adt::RuntimeException)
{
   Buffer buffer;

   setHandler(xmlNewTextWriterMemory(buffer, 0));
   setEncoding((const char*) document.getHandler()->encoding);

   try {
      document.compile(*this);
   }
   catch(adt::RuntimeException&) {
      releaseHandler();
      throw;
   }

   releaseHandler();

   return std::string(buffer.getValue());
}

std::string xml::Compiler::apply(const std::shared_ptr<Node>& node)
   throw(adt::RuntimeException)
{
   Buffer buffer;

   setHandler(xmlNewTextWriterMemory(buffer, 0));

   try {
      node->compile(*this);
   }
   catch(adt::RuntimeException&) {
      releaseHandler();
      throw;
   }

   releaseHandler();

   return std::string(buffer.getValue());
}

xml::Compiler::Buffer::Buffer()
{
   setHandler(xmlBufferCreate());
   setDeleter(xmlBufferFree);
}

const char* xml::Compiler::Buffer::getValue() const
   noexcept
{
   return(const char*) getHandler()->content;
}

const char* xml::Compiler::encode(const char* text)
   throw(adt::RuntimeException)
{
   bool needEncode  = false;

   if(m_encoding == NULL)
      return text;

   for(const char* ww = text; *ww != 0; ++ ww) {
      if(isascii(*ww) == false) {
         needEncode = true;
         break;
      }
   }

   if(needEncode == false )
      return text;

   xmlCharEncodingHandlerPtr handler = xmlFindCharEncodingHandler(m_encoding);

   if(handler == NULL) {
      COFFEE_THROW_EXCEPTION("Encoding '" << m_encoding << "' was not found");
   }

   int size = coffee_strlen(text) + 1;
   int outputSize = size * 2 + 1;
   int inputSize = size - 1;

   if(m_encodedReservedSize < outputSize) {
      if(m_encodedBuffer != NULL) {
         xmlFree(m_encodedBuffer);
         m_encodedBuffer = NULL;
      }
      m_encodedBuffer =(char*) xmlMalloc(m_encodedReservedSize = outputSize);
   }

   int rc = 0;

   rc = handler->input((xmlChar*) m_encodedBuffer, &outputSize,(const xmlChar *) text, &inputSize);

   if(rc < 0) {
      COFFEE_THROW_EXCEPTION("Can not encoding '" << text << "applying '" << m_encoding << "'");
   }

//   m_encodedBuffer =(char*) xmlRealloc(m_encodedBuffer, outputSize + 1);
   m_encodedBuffer [outputSize] = 0;

   return m_encodedBuffer;
}
