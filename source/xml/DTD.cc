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

#include <stdio.h>
#include <stdarg.h>

#include <libxml/parser.h>

#include <coffee/logger/Logger.hpp>

#include <coffee/xml/DTD.hpp>
#include <coffee/xml/Document.hpp>

using namespace coffee;

char xml::DTD::st_text [1024] = "";

xml::DTD::DTD () : Wrapper ()
{
   m_context = (xmlValidCtxtPtr) xmlMalloc (sizeof (xmlValidCtxt));
   m_context->error = callbackErrorHandler;
   m_context->warning = callbackWarningHandler;

   setDeleter (xmlFreeDtd);
   setNameExtractor(nameExtractor);
}

xml::DTD::~DTD()
{
   xmlFree (m_context);
}

//static
const char* xml::DTD::nameExtractor(const Handler handler)
   noexcept
{
   return (const char*) handler->name;
}

void xml::DTD::validate (const xml::Document* document) const
   throw (adt::RuntimeException)
{
   if (getHandler () == NULL)
      COFFEE_THROW_EXCEPTION("Method xml::DTD::initialize was not called");

   if (document->getHandler () == NULL)
      COFFEE_THROW_EXCEPTION("XML document was not parsed");

   xmlValidCtxtPtr context = const_cast <xmlValidCtxtPtr> (m_context);
   xml::Document::Handler doc = const_cast <xml::Document::Handler> (document->getHandler ());
   Handler dtd = const_cast <Handler> (getHandler ());

   if (xmlValidateDtd (context, doc, dtd) == 0)
      COFFEE_THROW_EXCEPTION("Document " << document->getName () << " does not match with DTD " << getName ());
}

/* static */
void xml::DTD::callbackErrorHandler (void *ctx,  const char *msg, ...)
   noexcept
{
   va_list ap;

   va_start (ap, msg);
   vsprintf (st_text, msg, ap);

   for (char* aux = st_text; *aux; aux ++)
      if (*aux == '\n')
         *aux = ' ';

   va_end (ap);

   LOG_ERROR (st_text);
}

/* static */
void xml::DTD::callbackWarningHandler (void *ctx,  const char *msg, ...)
   noexcept
{
   va_list ap;

   if (coffee::logger::Logger::wantsToProcess (coffee::logger::Level::Warning)) {
      va_start(ap, msg);
      vsprintf (st_text, msg, ap);

      for (char* aux = st_text; *aux; aux ++)
         if (*aux == '\n')
            *aux = ' ';
      va_end (ap);

      LOG_WARN(st_text);
   }
}
