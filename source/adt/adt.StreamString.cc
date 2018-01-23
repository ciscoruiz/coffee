// COFFEE - COmpany eFFEEctive Platform
//
// (c) Copyright 2018 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/coffee
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
#include <cctype> // for toupper
#include <string>
#include <algorithm>

#include <coffee/adt/StreamString.hpp>
#include <coffee/adt/AsString.hpp>
#include <coffee/adt/AsHexString.hpp>

using namespace std;
using namespace coffee;

void adt::StreamString::toUpper ()
   noexcept
{
   std::transform(begin(), end(), begin(), (int(*)(int)) toupper);
}

void adt::StreamString::toLower ()
   noexcept
{
   std::transform(begin(), end(), begin(), (int(*)(int)) tolower);
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


