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
#include <fcntl.h>
#include <unistd.h>
#include <fstream>

#include <libxml/parser.h>

#include <coffee/adt/DataBlock.hpp>
#include <coffee/logger/Logger.hpp>

#include <coffee/xml/DTD.hpp>
#include <coffee/xml/Document.hpp>

using namespace coffee;

xml::DTD::DTD () : Wrapper ()
{
   setDeleter (xmlFreeDtd);
}

xml::DTD::~DTD()
{
}

void xml::DTD::initialize (const boost::filesystem::path& filename)
   throw (adt::RuntimeException)
{
   _xmlDtd* result = nullptr;

   int stream;

   LOG_DEBUG ("Initializing DTD with file " << filename.c_str());

   if ((stream = open (filename.c_str(), O_RDONLY)) == -1) {
      COFFEE_THROW_EXCEPTION("Could not open to read " << filename.c_str());
   }

   close (stream);

   if ((result = xmlParseDTD (NULL, BAD_CAST(filename.c_str()))) == NULL) {
      COFFEE_THROW_EXCEPTION(filename.c_str() << " does not contains a valid DTD");
   }

   setHandler(result);
}

void xml::DTD::initialize (const adt::DataBlock& buffer)
   throw (adt::RuntimeException)
{
   char filename[] = "/tmp/mytemp.XXXXXX";
   int fd = mkstemp(filename);
   write(fd, buffer.data(), buffer.size());
   close(fd);

   try {
      const boost::filesystem::path file(filename);
      initialize(file);
      std::remove(filename);
   }
   catch(adt::RuntimeException&) {
      std::remove(filename);
      throw;
   }
}

void xml::DTD::validate (xml::Document& document) const
   throw (adt::RuntimeException)
{
   if (getHandler () == NULL)
      COFFEE_THROW_EXCEPTION("Method xml::DTD::initialize was not called");

   // See http://forums.devx.com/showthread.php?158033-Get-output-from-libxml2
   xmlValidCtxt context;
   char buffer[1024];
   coffee_memset(&context, 0, sizeof(context));
   context.userData=&buffer;
   context.error = callbackErrorHandler;
   context.warning = callbackWarningHandler;
   if (xmlValidateDtd (&context, document.getHandler(), getHandler()) == 0)
      COFFEE_THROW_EXCEPTION("Document " << document.getName() << " does not match with DTD ");
}

/* static */
void xml::DTD::callbackErrorHandler (void *ctx,  const char *msg, ...)
   noexcept
{
   va_list ap;
   va_start (ap, msg);
   vsprintf ((char*) ctx, msg, ap);
   va_end (ap);
   LOG_ERROR ((char*)ctx);
}

/* static */
void xml::DTD::callbackWarningHandler (void *ctx,  const char *msg, ...)
   noexcept
{
   va_list ap;
   va_start(ap, msg);
   vsprintf ((char*) ctx, msg, ap);
   va_end (ap);
   LOG_ERROR ((char*)ctx);
}
