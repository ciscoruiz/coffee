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

#include <coffee/dbms/ResultCode.hpp>
#include <coffee/dbms/ErrorCodeInterpreter.hpp>
#include <coffee/dbms/Database.hpp>

using namespace std;
using namespace coffee;

dbms::ResultCode::ResultCode(const dbms::Database& database, const int opCode) :
   m_errorCodeInterpreter(database.getErrorCodeInterpreter()),
   m_numericCode(opCode)
{
}

dbms::ResultCode::ResultCode(const std::shared_ptr<Database>& database, const int opCode) :
   m_errorCodeInterpreter(database->getErrorCodeInterpreter()),
   m_numericCode(opCode)
{
}

dbms::ResultCode::ResultCode(const dbms::Database& database, const int numericCode, const char* errorText) :
   m_errorCodeInterpreter(database.getErrorCodeInterpreter()),
   m_numericCode(numericCode),
   m_errorText(errorText)
{
}

bool dbms::ResultCode::notFound() const
   throw(adt::RuntimeException)
{
   if(!m_errorCodeInterpreter)
      COFFEE_THROW_EXCEPTION(asString() << " does not have associated database to decode error");
   
   return m_errorCodeInterpreter->notFound(m_numericCode);
}

bool dbms::ResultCode::successful() const
   throw(adt::RuntimeException)
{
   if(!m_errorCodeInterpreter)
      COFFEE_THROW_EXCEPTION(asString() << " does not have associated database to decode error");
   
   return m_errorCodeInterpreter->successful(m_numericCode);
}

bool dbms::ResultCode::locked() const
   throw(adt::RuntimeException)
{
   if(!m_errorCodeInterpreter)
      COFFEE_THROW_EXCEPTION(asString() << " does not have associated database to decode error");
   
   return m_errorCodeInterpreter->locked(m_numericCode);
}

bool dbms::ResultCode::lostConnection() const
   throw(adt::RuntimeException)
{
   if(!m_errorCodeInterpreter)
      COFFEE_THROW_EXCEPTION(asString() << " does not have associated database to decode error");
   
   return m_errorCodeInterpreter->lostConnection(m_numericCode);
}


adt::StreamString dbms::ResultCode::asString() const
   noexcept
{
   adt::StreamString result("dbms.ResultCode { Status=");

   if (successful()) {
      result << "Successful";
   }
   else if (notFound()) {
      result << "Not-Found";
   }
   else if (locked()) {
      result << "Locked";
   }
   else if (lostConnection()) {
      result << "Lost-Connection";
   }

   result << "(" << m_numericCode << ")";

   if (!m_errorText.empty()) {
      result << " | Comment=" << m_errorText;
   }

   return result << " }";
}
