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
