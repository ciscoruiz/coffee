#include <ctype.h>
#include <stdio.h>

#include <coffee/config/defines.hpp>

#include <coffee/adt/AsString.hpp>
#include <coffee/adt/DataBlock.hpp>

using namespace std;
using namespace coffee;

string adt::AsString::apply (const int number, const char* format)
   noexcept
{
   char aux [16];
   sprintf (aux, format, number);
   return string (aux);
}

string adt::AsString::apply (const unsigned int number)
   noexcept
{
   return to_string(number);
}

#ifndef __coffee64__
string adt::AsString::apply (const long number)
   noexcept
{
   return to_string(number);
}
#endif

string adt::AsString::apply (const Integer64 number)
   noexcept
{
   return to_string(number);
}

string adt::AsString::apply (const Unsigned64 number)
   noexcept
{
   return to_string(number);
}

string adt::AsString::apply (const float number, const char* format)
   noexcept
{
   char aux [64];
   sprintf (aux, format, number);
   return string (aux);
}

string adt::AsString::apply (const double number, const char* format)
   noexcept
{
   char aux [64];
   sprintf (aux, format, number);
   return string (aux);
}

std::string adt::AsString::apply (const DataBlock& dataBlock, const int characterByLine)
   noexcept
{
   char aux [8];
   std::string numbers;
   std::string characters;
   unsigned char c;
   size_t i;
   size_t maxi = dataBlock.size();
   std::string result;

   if (dataBlock.empty () == true) {
      result = "<null>";
      return result;
   }

   result += " (";
//   result += functions::asHexString (nemesis_ptrnumber_cast (a_buffer));
   result += "):";

   for (i = 0; i < maxi; ++ i) {
      if ((i % characterByLine) == 0) {
         if (i > 0) {
            result.append ("\n").append (numbers).append (" ").append (characters);
            characters.clear ();
         }
         sprintf (aux, "%4zd: ", i);
         numbers = aux;
      }

      c = (unsigned char) dataBlock [i];
      sprintf (aux, "%02x ", c);
      numbers += aux;
      characters += (isprint (c) ? (char) c: '.');
   }

   result += '\n';
   result += numbers;
   while ((i % characterByLine) != 0) {
      result += "   ";
      i ++;
   }
   result += "  ";
   return result += characters;
}
