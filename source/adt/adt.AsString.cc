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
