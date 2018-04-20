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

#include <map>

#include <coffee/http/HttpResponse.hpp>
#include <coffee/http/HttpRequest.hpp>

using namespace coffee;

http::HttpResponse::HttpResponse(const int statusCode, const std::shared_ptr<HttpRequest>& request) :
   http::HttpMessage(request->getMajorVersion(), request->getMinorVersion()),
   m_statusCode(statusCode),
   m_request(request)
{
}

std::string http::HttpResponse::encodeFirstLine() const
   throw(basis::RuntimeException)
{
   static std::map<int,const char*> reasons = {
      { 100, "Continue" },
      { 101, "Switching Protocols" },
      { 200, "OK" },
      { 201, "Created" },
      { 202, "Accepted" },
      { 203, "Non-Authoritative Information" },
      { 204, "No Content" },
      { 205, "Reset Content" },
      { 206, "Partial Content" },
      { 300, "Multiple Choise" },
      { 301, "Moved Permanently" },
      { 302, "Found" },
      { 303, "See Other" },
      { 304, "Not Modified" },
      { 305, "Use proxy" },
      { 307, "Temporary Redirect" },
      { 400, "Bad Request" },
      { 401, "Unautorized" },
      { 402, "Payment Required" },
      { 403, "Forbidden" },
      { 404, "Not found" },
      { 405, "Method Not Allowed" },
      { 406, "Not Acceptable" },
      { 407, "Proxy Authentication Required" },
      { 408, "Request Time-out" },
      { 409, "Conflict" },
      { 410, "Gone" },
      { 411, "Length Required" },
      { 412, "Precondition Failed" },
      { 413, "Request Entity Too Large" },
      { 414, "Request-URI Too Large" },
      { 415, "Unsupported Media Type" },
      { 416, "Requested range not satisfiable" },
      { 417, "Expectation Failed" },
      { 500, "Internal Server Error" },
      { 501, "Not Implemented" },
      { 502, "Bad Gateway" },
      { 503, "Service Unavailable" },
      { 504, "Gateway Time-out" },
      { 505, "HTTP Version not supported" }
   };

   basis::StreamString ss;

   const char* reason = "Unknown";

   auto ii = reasons.find(m_statusCode);

   if (ii != reasons.end())
      reason = ii->second;

   return ss << encodeVersion() << " " << m_statusCode << " " << reason;
}

