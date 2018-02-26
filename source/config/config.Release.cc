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

#include <stdio.h>
#include <sys/utsname.h>

#include <coffee/config/Release.hpp>
#include <coffee/config/defines.hpp>

#include <stdio.h>

using namespace std;
using namespace coffee;

string config::Release::getVersion () noexcept
{
   static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

   // https://stackoverflow.com/questions/1765014/convert-string-from-date-into-a-time-t
   char s_month[4];
   int day, year;

   sscanf(__DATE__, "%s %d %d", s_month, &day, &year);

   const char* index = coffee_strstr(month_names, s_month);
   const int month = (index != NULL) ? ((index - month_names)/3) + 1: 0;

   char version[24];
   sprintf(version, "%02d.%02d.%02d", year - 2000, month, day);
   std::string result(version);
   result += getArchitecture ();
   return result;
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
