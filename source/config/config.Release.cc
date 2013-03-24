#include <stdio.h>
#include <sys/utsname.h>

#include <wepa/config/Release.hpp>
#include <wepa/config/defines.hpp>

using namespace std;
using namespace wepa;

string config::Release::getVersion () throw ()
{
   static const int version = WEPA_VERSION;

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
string config::Release::getArchitecture () throw ()
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

   char* release = wepa_strchr (un.release, '.');              // (1)
   if (release != NULL)
      if ((release = wepa_strchr (release + 1, '.')) != NULL)
         *release = 0;

   result += un.release;
   result += " (";
   result += un.machine;
   result += ")";

   return result;
}
