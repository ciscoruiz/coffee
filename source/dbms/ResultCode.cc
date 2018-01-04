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

#include <wepa/dbms/Database.hpp>
#include <wepa/dbms/ResultCode.hpp>

using namespace std;
using namespace wepa;

bool dbms::ResultCode::notFound () const
   throw (adt::RuntimeException)
{
   if (m_database == NULL)
      WEPA_THROW_EXCEPTION(asString () << " does not have associated database to decode error");
   
   return m_database->notFound (m_numericCode);
}

bool dbms::ResultCode::successful () const
   throw (adt::RuntimeException)
{
   if (m_database == NULL)
      WEPA_THROW_EXCEPTION(asString () << " does not have associated database to decode error");
   
   return m_database->successful (m_numericCode);
}

bool dbms::ResultCode::locked () const
   throw (adt::RuntimeException)
{
   if (m_database == NULL)
      WEPA_THROW_EXCEPTION(asString () << " does not have associated database to decode error");
   
   return m_database->locked (m_numericCode);
}

bool dbms::ResultCode::lostConnection () const
   throw (adt::RuntimeException)
{
   if (m_database == NULL)
      WEPA_THROW_EXCEPTION(asString () << " does not have associated database to decode error");
   
   return m_database->lostConnection (m_numericCode);
}

//
// No usamos la std::string porque la gran mayor�a de las veces la ejecuci�n de las sentencias SQL ser�
// correcta => no har� falta reservar ninguna memoria.
//
void dbms::ResultCode::copy (const char* text)
   noexcept
{
   if (text == NULL) {
      if (m_errorText != NULL) {
         free (m_errorText);
         m_errorText = NULL;
      }
   }
   else {
      char* aux;
      if ((aux = wepa_strchr ((char*) text, '\n')) != NULL)
         *aux = 0;

      const int textLen = wepa_strlen (text);
      
      if (m_errorText == NULL)
         m_errorText = strdup (text);
      else if (wepa_strlen (m_errorText) >= textLen)
         wepa_strcpy (m_errorText, text);
      else {
         free (m_errorText);
         m_errorText = strdup (text);
      }
   }
}

adt::StreamString dbms::ResultCode::asString () const
   noexcept
{
   adt::StreamString result ("dbms.ResultCode { NumericCode=");
   result << m_numericCode;
   result << " | Comment=" << ((m_errorText == NULL) ? "(null)": m_errorText);
   result << " | Success=";

   if (m_database != NULL)
      result << successful ();
   else 
      result << "<No ErrorDecoder>";

   return result << " }";
}
