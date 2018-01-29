#include <stdio.h>
#include <sys/utsname.h>

#include <coffee/config/Release.hpp>
#include <coffee/config/defines.hpp>

using namespace std;
using namespace coffee;

string config::Release::getVersion () noexcept
{
   static const int version = COFFEE_VERSION;

   string result;
   int mainVersion = (version & 0xff0000) >> 16;
   int year = (version & 0xff00) >> 8;
   int month = (version & 0xff);
   char aux [32];
   sprintf (aux, "%d.%02d.%02d", mainVersion, year, month);
   result = aux;
   return result += getArchitecture ();
}

// (1) It only will use the main OS version
string config::Release::getArchitecture () noexcept
{
   string result;

   #ifdef _MT
      result = "/MT";
   #else
      result = "/ST";
   #endif

   #ifdef _DEBUG
      result += "/D";
   #else
      result += "/O";
   #endif

   struct utsname un;

   uname (&un);

   result += '/';
   result += un.sysname;
   result += ' ';

   char* release = coffee_strchr (un.release, '.');              // (1)
   if (release != NULL)
      if ((release = coffee_strchr (release + 1, '.')) != NULL)
         *release = 0;

   result += un.release;
   result += " (";
   result += un.machine;
   result += ")";

   return result;
}
