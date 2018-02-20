
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
