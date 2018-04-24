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
#include <algorithm>

#include <coffee/http/protocol/state/HttpProtocolWaitingMessage.hpp>
#include <coffee/http/protocol/HttpProtocolDecoder.hpp>
#include <coffee/http/protocol/defines.hpp>
#include <coffee/http/HttpResponse.hpp>
#include <coffee/http/HttpRequest.hpp>
#include <coffee/http/url/URLParser.hpp>
#include <coffee/logger/Logger.hpp>

using namespace coffee;
using namespace coffee::http::protocol::state;

HttpProtocolState::ProcessResult::_v HttpProtocolWaitingMessage::process(HttpProtocolDecoder& context, const Token& token) const
   throw(basis::RuntimeException)
{
   auto items = http::protocol::split(token.value, ' ');

   std::shared_ptr<http::HttpMessage> message = tryResponse(items);

   if (!message) {
      message = tryRequest(items);
   }

   if (message) {
      context.m_result = message;
      context.setState(HttpProtocolDecoder::State::WaitingContentLength);
   }

   return ProcessResult::Continue;
}

std::shared_ptr<http::HttpMessage> HttpProtocolWaitingMessage::tryResponse(const std::vector<std::string>& items)
   noexcept
{
   static std::shared_ptr<http::HttpMessage> empty;

   if (items.size() < 3) {
      return empty;
   }

   if (strncasecmp(items[0].c_str(), "http", 4) != 0) {
      return empty;
   }

   const std::string& statusCode(items[1]);

   if (!protocol::isNumeric(statusCode))
      return empty;

   std::pair<uint16_t, uint16_t> httpVersion(0, 0);

   if (!tryHttpVersion(items[0], httpVersion)){
      return empty;
   }

   return std::shared_ptr<http::HttpMessage>(new http::HttpResponse(httpVersion.first, httpVersion.second, atoi(statusCode.c_str())));
}

std::shared_ptr<http::HttpMessage> HttpProtocolWaitingMessage::tryRequest(const std::vector<std::string>& items)
   noexcept
{
   static std::shared_ptr<http::HttpMessage> empty;

   if (items.size() < 3) {
      return empty;
   }

   HttpRequest::Method::_v method;

   if (!tryMethod(items[0], method)) {
      return empty;
   }

   std::shared_ptr<url::URL> url = tryURL(items[1]);

   if (!url) {
      return empty;
   }

   std::pair<uint16_t, uint16_t> httpVersion(0, 0);

   if (!tryHttpVersion(items[2], httpVersion)){
      return empty;
   }

   return http::HttpRequest::instantiate(method, url, httpVersion.first, httpVersion.second);
}

bool HttpProtocolWaitingMessage::tryHttpVersion(const std::string& item, std::pair<uint16_t, uint16_t>& httpVersion)
   noexcept
{
   auto strVersions = protocol::separate(protocol::separate(item, '/').second, '.');

   if (!isNumeric(strVersions.first) || !isNumeric(strVersions.second))
      return false;

   httpVersion.first = atoi(strVersions.first.c_str());
   httpVersion.second = atoi(strVersions.second.c_str());

   return true;
}

bool HttpProtocolWaitingMessage::tryMethod(const std::string& item, HttpRequest::Method::_v& value)
   noexcept
{
   const char* methodName = item.c_str();

   for (int method = 0; protocol::requestMethodNames[method] != nullptr; ++ method) {
      if (strcasecmp(methodName, protocol::requestMethodNames[method]) == 0) {
         value = (HttpRequest::Method::_v) method;
         return true;
      }
   }

   return false;
}

std::shared_ptr<http::url::URL> HttpProtocolWaitingMessage::tryURL(const std::basic_string<char>& item)
   noexcept
{
   static std::shared_ptr<http::url::URL> empty;

   url::URLParser urlParser(item);

   try {
      return urlParser.build();
   }
   catch(basis::RuntimeException& ex){
      logger::Logger::write(ex);
      return empty;
   }
}
