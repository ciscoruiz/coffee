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

#include <string.h>

#include <coffee/http/protocol/defines.hpp>
#include <algorithm>

namespace coffee {
namespace http {
namespace protocol {
//static
const char newLineCharacters[] = { 13, 10, 0 };

//static
const char* requestMethodNames[] = {
   "PORT", "OPTIONS", "GET", "HEAD", "PUT", "DELETE", "TRACE", "CONNECT", nullptr
};

const std::string headerNames[] = {
   "Cache-Control", "Connection", "Date", "Pragma", "Trailer", "Transfer-Encoding",
   "Upgrade", "Via", "Warning", "Accept", "Accept-Charset", "Accept-Encoding",
   "Accept-Language", "Authorization", "Expect", "From", "Host", "If-Match", "If-Modified-Since",
   "If-None-Match", "If-Range", "If-Unmodified-Since", "Max-Forwards", "Proxy-Authorization",
   "Range", "Referer", "TE", "User-Agent", "Allow", "Content-Encoding", "Content-Language",
   "Content-Length", "Content-Location", "Content-MD5", "Content-Range", "Content-Type",
   "Expires", "Last-Modified", "Accept-Ranges", "Age", "ETAG", "Location", "Proxy-Authenticate",
   "Retry-After", "Server", "Vary", "WWW-Authenticate", ""
};

std::pair<std::string, std::string> separate(const std::string& string, const char delim)
   noexcept
{
   std::pair<std::string, std::string> result;

   auto pos = string.find(delim);

   if (pos != std::string::npos) {
      result.first = string.substr(0, pos);
      result.second = string.substr(pos + 1);
   }
   else {
      result.first = string;
   }

   return result;
}

//static
std::vector<std::string> split(const std::string& string, const char _delim)
   noexcept
{
   char delim[2] = { _delim, 0 };

   std::vector<std::string> result;

   char* source = (char*) string.c_str();
   const char* item;
   char* saveptr = nullptr;

   while ((item = strtok_r(source, delim, &saveptr)) != nullptr) {
      source = nullptr;
      result.push_back(item);
   }

   return result;
}

bool isNumeric(const std::string& str) noexcept
{
   if (str.empty())
      return false;

   auto ii = std::find_if_not(str.begin(), str.end(), [](unsigned char cc) { return std::isdigit(cc); });
   return ii == str.end();
}

}
}
}
