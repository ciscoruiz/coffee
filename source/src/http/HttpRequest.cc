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

#include <coffee/http/HttpRequest.hpp>
#include <coffee/http/protocol/defines.hpp>
#include <coffee/http/url/URLParser.hpp>

using namespace coffee;

//static
std::shared_ptr<http::HttpRequest> http::HttpRequest::instantiate(const http::HttpRequest::Method::_v method, const std::string& url, const uint32_t majorVersion, const uint32_t minorVersion)
   throw(basis::RuntimeException)
{
   http::url::URLParser parser(url);

   std::shared_ptr<http::HttpRequest> result(new http::HttpRequest(method, parser.build(), majorVersion, minorVersion));
   return result;
}

std::string http::HttpRequest::encodeFirstLine() const
   throw(basis::RuntimeException)
{
   basis::StreamString ss;

   return ss << Method::asString(m_method) << " " << m_url->encode() << " " << encodeVersion();
}

//static
const char* http::HttpRequest::Method::asString(const http::HttpRequest::Method::_v method)
   noexcept
{
   return protocol::requestMethodNames[method];
}
