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

const int adt::AsString::DefaultCharactersByLine = 16;

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

string adt::AsString::apply (const int64_t number)
   noexcept
{
   return to_string(number);
}

string adt::AsString::apply (const uint64_t number)
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

std::string adt::AsString::apply(const std::chrono::seconds& value)
   noexcept
{
   std::string result(AsString::apply(value.count()));
   return result += " sec";
}

std::string adt::AsString::apply(const std::chrono::seconds& value, const char* format)
   noexcept
{
   char aux[64];

   time_t time = value.count();

   struct tm* tt = localtime (&time);

   if (tt == nullptr) {
      COFFEE_THROW_EXCEPTION(value << " is a bad time");
   }

   if (strftime (aux, sizeof (aux), format, tt) == 0) {
      COFFEE_THROW_EXCEPTION(value << "is a bad date");
   }

   return string(aux);
}

std::string adt::AsString::apply(const std::chrono::milliseconds& value)
   noexcept
{
   std::string result(AsString::apply(value.count()));
   return result += " ms";
}

std::string adt::AsString::apply(const std::chrono::microseconds& value)
   noexcept
{
   std::string result(AsString::apply(value.count()));
   return result += " us";
}


