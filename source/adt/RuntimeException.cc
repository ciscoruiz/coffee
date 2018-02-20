#include <coffee/adt/RuntimeException.hpp>
#include <coffee/adt/StreamString.hpp>

using namespace std;
using namespace coffee;

string adt::RuntimeException::asString() const noexcept
{
   StreamString str (this->filePosition());

   if (m_errorCode != NullErrorCode) {
      str << "ErrorCode: " << m_errorCode << " | ";
   }

   str << what ();

   return str;
}

