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

dbms::ResultCode::ResultCode(const dbms::Database& database, const int numericCode, const char* errorText) :
   m_errorCodeInterpreter(database.getErrorCodeInterpreter()),
   m_numericCode(numericCode),
   m_errorText(errorText)
{
}

bool dbms::ResultCode::notFound() const
   throw(basis::RuntimeException)
{
   if(!m_errorCodeInterpreter)
      COFFEE_THROW_EXCEPTION(asString() << " does not have associated database to decode error");
   
   return m_errorCodeInterpreter->notFound(m_numericCode);
}

bool dbms::ResultCode::successful() const
   throw(basis::RuntimeException)
{
   if(!m_errorCodeInterpreter)
      COFFEE_THROW_EXCEPTION(asString() << " does not have associated database to decode error");
   
   return m_errorCodeInterpreter->successful(m_numericCode);
}

bool dbms::ResultCode::locked() const
   throw(basis::RuntimeException)
{
   if(!m_errorCodeInterpreter)
      COFFEE_THROW_EXCEPTION(asString() << " does not have associated database to decode error");
   
   return m_errorCodeInterpreter->locked(m_numericCode);
}

bool dbms::ResultCode::lostConnection() const
   throw(basis::RuntimeException)
{
   if(!m_errorCodeInterpreter)
      COFFEE_THROW_EXCEPTION(asString() << " does not have associated database to decode error");
   
   return m_errorCodeInterpreter->lostConnection(m_numericCode);
}


basis::StreamString dbms::ResultCode::asString() const
   noexcept
{
   basis::StreamString result("dbms.ResultCode { Status=");

   if (m_errorCodeInterpreter) {
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
   }

   result << "(" << m_numericCode << ")";

   if (!m_errorText.empty()) {
      result << " | Comment=" << m_errorText;
   }

   return result << " }";
}
