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

#include <wepa/adt/AsHexString.hpp>
#include <wepa/adt/DataBlock.hpp>

using namespace std;
using namespace wepa;

string adt::AsHexString::apply (const int number)
   noexcept
{
   char aux [16];
   sprintf (aux, "0x%x", number);
   return string (aux);
}

string adt::AsHexString::apply (const unsigned int number)
   noexcept
{
   char aux [16];
   sprintf (aux, "0x%x", number);
   return string (aux);
}

string adt::AsHexString::apply (const Integer64 number)
   noexcept
{
   char aux [24];
#ifdef __wepa64__
   sprintf (aux, "0x%lx", number);
#else
   sprintf (aux, "0x%llx", number);
#endif

   return string (aux);
}

string adt::AsHexString::apply (const Unsigned64 number)
   noexcept
{
   char aux [32];
#ifdef __wepa64__
   sprintf (aux, "0x%lx", number);
#else
   sprintf (aux, "0x%llx", number);
#endif
   return string (aux);
}

