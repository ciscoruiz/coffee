// COFFEE - COmpany eFFEEctive Platform
//
//(c) Copyright 2018 Francisco Ruiz Rayo
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
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

std::string xml::Compiler::apply(std::shared_ptr<Document> document)
   throw(adt::RuntimeException)
{
   Buffer buffer;

   setHandler(xmlNewTextWriterMemory(buffer, 0));
   setEncoding((const char*) document->getHandler()->encoding);

   try {
      document->compile(*this);
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
