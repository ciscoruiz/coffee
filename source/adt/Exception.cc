#include <coffee/adt/Exception.hpp>
#include <coffee/adt/StreamString.hpp>

using namespace std;
using namespace coffee;

std::string adt::Exception::filePosition () const
   noexcept
{
   StreamString str;

   str << "[" << m_fromFile << "(" << m_fromLine << "): " << m_fromMethod << "] ";

   return str;
}
