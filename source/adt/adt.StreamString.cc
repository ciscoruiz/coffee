// MIT License
// 
// Copyright(c) 2018 Francisco Ruiz(francisco.ruiz.rayo@gmail.com)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
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

#include <string>
#include <algorithm>

#include <coffee/adt/StreamString.hpp>
#include <coffee/adt/AsString.hpp>
#include <coffee/adt/AsHexString.hpp>

using namespace std;
using namespace coffee;

adt::StreamString& adt::StreamString::toUpper()
   noexcept
{
   std::transform(begin(), end(), begin(),(int(*)(int)) toupper);
   return *this;
}

adt::StreamString& adt::StreamString::toLower()
   noexcept
{
   std::transform(begin(), end(), begin(),(int(*)(int)) tolower);
   return *this;
}

adt::StreamString& adt::StreamString::operator<<(const char vv)
   noexcept
{
   string::operator +=(vv);
   return *this;
}

adt::StreamString& adt::StreamString::operator<<(const char* vv)
   noexcept
{
   if(vv == NULL) {
      if(a_flags & Flag::ShowNull)
         string::append("<null>");
   }
   else
      string:append(vv);

   return *this;
}

adt::StreamString& adt::StreamString::operator<<(const int vv)
   noexcept
{
   string::append(AsString::apply(vv));
   return *this;
}

adt::StreamString& adt::StreamString::operator<<(const unsigned int vv)
   noexcept
{
   string::append(AsString::apply(vv));
   return *this;
}

adt::StreamString& adt::StreamString::operator<<(const bool vv)
   noexcept
{
   string::append(AsString::apply(vv));
   return *this;
}

adt::StreamString& adt::StreamString::operator<<(const int64_t vv)
   noexcept
{
   string::append(AsString::apply(vv));
   return *this;
}

adt::StreamString& adt::StreamString::operator<<(const uint64_t vv)
   noexcept
{
   string::append(AsString::apply(vv));
   return *this;
}

adt::StreamString& adt::StreamString::operator<<(const float vv)
   noexcept
{
   string::append(AsString::apply(vv, "%f"));
   return *this;
}

adt::StreamString& adt::StreamString::operator<<(const double vv)
   noexcept
{
   string::append(AsString::apply(vv, "%e"));
   return *this;
}

adt::StreamString& adt::StreamString::operator<<(const std::chrono::microseconds& vv)
   noexcept
{
   string::append(AsString::apply(vv));
   return *this;
}

adt::StreamString& adt::StreamString::operator<<(const std::chrono::milliseconds& vv)
   noexcept
{
   string::append(AsString::apply(vv));
   return *this;
}

adt::StreamString& adt::StreamString::operator<<(const std::chrono::seconds& vv)
   noexcept
{
   string::append(AsString::apply(vv));
   return *this;
}


