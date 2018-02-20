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

#include <coffee/adt/AsHexString.hpp>
#include <coffee/adt/DataBlock.hpp>

using namespace std;
using namespace coffee;

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
#ifdef __coffee64__
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
#ifdef __coffee64__
   sprintf (aux, "0x%lx", number);
#else
   sprintf (aux, "0x%llx", number);
#endif
   return string (aux);
}

