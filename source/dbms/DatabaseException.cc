#include <coffee/dbms/DatabaseException.hpp>
#include <coffee/adt/StreamString.hpp>

using namespace std;
using namespace coffee;

string dbms::DatabaseException::asString() const noexcept
{
   adt::StreamString str (this->filePosition ());

   str << "ResultCode=" << m_resultCode.asString () << " | ";

   str << what ();

   return str;
}

