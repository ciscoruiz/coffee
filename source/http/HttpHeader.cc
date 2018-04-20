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

#include <coffee/http/HttpHeader.hpp>

using namespace coffee;

namespace coffee {
namespace http {
static std::string headerNames[] = {
   "Cache-Control", "Connection", "Date", "Pragma", "Trailer", "Transfer-Encoding",
   "Upgrade", "Via", "Warning", "Accept", "Accept-Charset", "Accept-Encoding",
   "Accept-Language", "Authorization", "Expect", "From", "Host", "If-Match", "If-Modified-Since",
   "If-None-Match", "If-Range", "If-Unmodified-Since", "Max-Forwards", "Proxy-Authorization",
   "Range", "Referer", "TE", "User-Agent", "Allow", "Content-Encoding", "Content-Language",
   "Content-Length", "Content-Location", "Content-MD5", "Content-Range", "Content-Type",
   "Expires", "Last-Modified", "Accept-Ranges", "Age", "ETAG", "Location", "Proxy-Authenticate",
   "Retry-After", "Server", "Vary", "WWW-Authenticate"
};
}
}

//virtual
const std::string& http::HttpHeader::getName () const
   throw(basis::RuntimeException)
{
   if (m_type == Type::Custom) {
      COFFEE_THROW_EXCEPTION("HttpHeader of type none can not be instantiated for users");
   }
   return Type::asString(m_type);
}

//static
const std::string& http::HttpHeader::Type::asString(const Type::_v type)
   noexcept
{
   static const std::string custom("Custom");

   return (type == Type::Custom) ? custom: headerNames[type];
}
