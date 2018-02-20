#include <string>
#include <algorithm>

#include <coffee/adt/StreamString.hpp>
#include <coffee/adt/AsString.hpp>
#include <coffee/adt/AsHexString.hpp>

using namespace std;
using namespace coffee;

adt::StreamString& adt::StreamString::toUpper ()
   noexcept
{
   std::transform(begin(), end(), begin(), (int(*)(int)) toupper);
   return *this;
}

adt::StreamString& adt::StreamString::toLower ()
   noexcept
{
   std::transform(begin(), end(), begin(), (int(*)(int)) tolower);
   return *this;
}

adt::StreamString& adt::StreamString::operator<< (const char vv)
   noexcept
{
   string::operator +=(vv);
   return *this;
}

adt::StreamString& adt::StreamString::operator<< (const char* vv)
   noexcept
{
   if (vv == NULL) {
      if (a_flags & Flag::ShowNull)
         string::append ("<null>");
   }
   else
      string:append (vv);

   return *this;
}

adt::StreamString& adt::StreamString::operator<< (const int vv)
   noexcept
{
   string::append(AsString::apply (vv));
   return *this;
}

adt::StreamString& adt::StreamString::operator<< (const unsigned int vv)
   noexcept
{
   string::append(AsString::apply (vv));
   return *this;
}

adt::StreamString& adt::StreamString::operator<< (const bool vv)
   noexcept
{
   string::append(AsString::apply (vv));
   return *this;
}

adt::StreamString& adt::StreamString::operator<< (const Integer64 vv)
   noexcept
{
   string::append(AsString::apply (vv));
   return *this;
}

adt::StreamString& adt::StreamString::operator<< (const Unsigned64 vv)
   noexcept
{
   string::append(AsString::apply (vv));
   return *this;
}

adt::StreamString& adt::StreamString::operator<< (const float vv)
   noexcept
{
   string::append(AsString::apply (vv, "%f"));
   return *this;
}

adt::StreamString& adt::StreamString::operator<< (const double vv)
   noexcept
{
   string::append(AsString::apply (vv, "%e"));
   return *this;
}


