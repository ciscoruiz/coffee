// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2018 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/wepa
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#include <ctype.h>
#include <stdio.h>

#include <wepa/config/defines.hpp>

#include <wepa/adt/AsString.hpp>
#include <wepa/adt/DataBlock.hpp>

using namespace std;
using namespace wepa;

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
   char aux [16];
   sprintf (aux, "%u", number);
   return string (aux);
}

#ifndef __wepa64__
string adt::AsString::apply (const long number)
   noexcept
{
   char aux [32];
   sprintf (aux, "%ld", number);
   return string (aux);
}
#endif

string adt::AsString::apply (const Integer64 number)
   noexcept
{
   char aux [24];
#ifdef __wepa64__
   sprintf (aux, "%ld", number);
#else
   sprintf (aux, "%lld", number);
#endif

   return string (aux);
}

string adt::AsString::apply (const Unsigned64 number)
   noexcept
{
   char aux [32];
#ifdef __wepa64__
   sprintf (aux, "%lu", number);
#else
   sprintf (aux, "%llu", number);
#endif
   return string (aux);
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
