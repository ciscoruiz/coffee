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

#include <gtest/gtest.h>

#include <coffee/http/HttpRequest.hpp>
#include <coffee/http/HttpResponse.hpp>
#include <coffee/http/protocol/defines.hpp>
#include <coffee/http/protocol/HttpProtocolDecoder.hpp>
#include <coffee/http/protocol/HttpProtocolEncoder.hpp>

using namespace coffee;
using namespace coffee::http;

std::shared_ptr<HttpMessage> encode(std::shared_ptr<HttpMessage> message)
{
   protocol::HttpProtocolEncoder encoder;
   protocol::HttpProtocolDecoder decoder;

   return decoder.apply(encoder.apply(message));
}

TEST(HttpProtocolDecoder, basic_request )
{
   auto message = encode(HttpRequest::instantiate(HttpRequest::Method::Connect, "/uri/res"));

   ASSERT_TRUE(message != nullptr);
   ASSERT_EQ(1, message->getMajorVersion());
   ASSERT_EQ(1, message->getMinorVersion());

   {
      auto response = std::dynamic_pointer_cast<http::HttpResponse>(message);
      ASSERT_TRUE(!response);
   }

   auto request = std::dynamic_pointer_cast<http::HttpRequest>(message);

   ASSERT_EQ(HttpRequest::Method::Connect, request->getMethod());
   ASSERT_EQ("/uri/res", request->getPath());
   ASSERT_TRUE(request->getBody().empty());
}

TEST(HttpProtocolDecoder, basic_response )
{
   auto response = HttpResponse::instantiate(HttpRequest::instantiate(HttpRequest::Method::Connect, "/uri"));
   response->setStatusCode(401);

   auto message = encode(response);

   ASSERT_TRUE(message != nullptr);
   ASSERT_EQ(1, message->getMajorVersion());
   ASSERT_EQ(1, message->getMinorVersion());

   {
      auto request = std::dynamic_pointer_cast<http::HttpRequest>(message);
      ASSERT_TRUE(!request);
   }

   response = std::dynamic_pointer_cast<http::HttpResponse>(message);

   ASSERT_EQ(401, response->getStatusCode());
   ASSERT_TRUE(response->getBody().empty());
}

TEST(HttpProtocolDecoder, header_request )
{
   auto request = HttpRequest::instantiate(HttpRequest::Method::Get, "/URI", 2);

   auto response = HttpResponse::instantiate(request);
   response->setStatusCode(200).setErrorDescription("Good!").setHeader(HttpHeader::Type::Age, "1234").setHeader(HttpHeader::Type::AcceptLanguage, "hava/x").setCustomHeader("MyHeader", "hello world");

   auto message = encode(response);

   ASSERT_TRUE(message != nullptr);
   ASSERT_EQ(2, message->getMajorVersion());
   ASSERT_EQ(1, message->getMinorVersion());

   response = std::dynamic_pointer_cast<http::HttpResponse>(message);

   ASSERT_TRUE(response != nullptr);
   ASSERT_EQ(200, response->getStatusCode());
   ASSERT_TRUE(response->isOk());
   ASSERT_EQ("Good!", response->getErrorDescription());
   ASSERT_TRUE(response->hasHeader(HttpHeader::Type::Age));
   ASSERT_TRUE(response->hasCustomHeader("MyHeader"));
   ASSERT_TRUE(!response->hasHeader(HttpHeader::Type::AcceptEncoding));
   ASSERT_TRUE(!response->hasCustomHeader("OtherMyHeader"));
   ASSERT_EQ("1234", response->getHeaderValue(HttpHeader::Type::Age));
   ASSERT_EQ("hava/x", response->getHeaderValue(HttpHeader::Type::AcceptLanguage));
   ASSERT_EQ("hello world", response->getCustomHeaderValue("MyHeader"));
   ASSERT_TRUE(response->getBody().empty());
}

TEST(HttpProtocolDecoder, header_body_request )
{
   char memory[1024];
   basis::DataBlock body(memory,sizeof(memory));

   auto request = HttpRequest::instantiate(HttpRequest::Method::Head, "/uri/res", 2);
   request->setHeader(HttpHeader::Type::Age, "1234").setHeader(HttpHeader::Type::Age, "333").setBody(body);

   auto message = encode(request);

   ASSERT_TRUE(message != nullptr);
   ASSERT_EQ(2, message->getMajorVersion());
   ASSERT_EQ(1, message->getMinorVersion());

   request = std::dynamic_pointer_cast<http::HttpRequest>(message);

   ASSERT_TRUE(request != nullptr);
   ASSERT_EQ(HttpRequest::Method::Head, request->getMethod());
   ASSERT_EQ("/uri/res", request->getPath());
   ASSERT_EQ("1234,333", request->getHeaderValue(HttpHeader::Type::Age));
   ASSERT_EQ("1024", request->getHeaderValue(HttpHeader::Type::ContentLength));
   ASSERT_EQ(1024, request->getBody().size());
   ASSERT_TRUE(memcmp(request->getBody().data(), memory, 1024) == 0);
}

TEST(HttpProtocolDecoder, header_body_response )
{
   char memory[1024];
   basis::DataBlock body(memory,sizeof(memory));

   auto request = HttpRequest::instantiate(HttpRequest::Method::Head, "/path/resource", 2, 2);
   auto response = HttpResponse::instantiate(request);
   response->setStatusCode(1022).setCustomHeader("MyHeader", "1234").setCustomHeader("MyHeader", "333").setBody(body);

   auto message = encode(response);

   ASSERT_TRUE(message != nullptr);
   ASSERT_EQ(2, message->getMajorVersion());
   ASSERT_EQ(2, message->getMinorVersion());

   response = std::dynamic_pointer_cast<http::HttpResponse>(message);

   ASSERT_TRUE(response != nullptr);
   ASSERT_EQ(1022, response->getStatusCode());
   ASSERT_EQ("Unknown", response->getErrorDescription());
   ASSERT_EQ("1234,333", response->getCustomHeaderValue("MyHeader"));
   ASSERT_EQ("1024", response->getHeaderValue(HttpHeader::Type::ContentLength));
   ASSERT_EQ(1024, response->getBody().size());
   ASSERT_TRUE(memcmp(response->getBody().data(), memory, 1024) == 0);
}

TEST(HttpProtocolDecoder, request_header_after_contentlength )
{
   char memory[1024];
   basis::DataBlock body(memory, sizeof(memory));

   auto request = HttpRequest::instantiate(HttpRequest::Method::Head, "/the:uri", 2);
   request->setHeader(HttpHeader::Type::Age, "1234").setBody(body);
   request->setHeader(HttpHeader::Type::ContentLength, "1024").setHeader(HttpHeader::Type::Allow, "allow");

   auto message = encode(request);

   ASSERT_TRUE(message != nullptr);
   ASSERT_EQ(2, message->getMajorVersion());
   ASSERT_EQ(1, message->getMinorVersion());

   request = std::dynamic_pointer_cast<http::HttpRequest>(message);

   ASSERT_TRUE(request != nullptr);
   ASSERT_TRUE(request->hasHeader(HttpHeader::Type::Allow));
   ASSERT_EQ("allow", request->getHeaderValue(HttpHeader::Type::Allow));
   ASSERT_EQ("1024", request->getHeaderValue(HttpHeader::Type::ContentLength));
   ASSERT_EQ(1024, request->getBody().size());
   ASSERT_TRUE(memcmp(request->getBody().data(), memory, 1024) == 0);
}

TEST(HttpProtocolDecoder, request_custom_header_after_contentlength )
{
   char memory[1024];
   basis::DataBlock body(memory, sizeof(memory));

   auto request = HttpRequest::instantiate(HttpRequest::Method::Head, "/uri", 2);
   request->setHeader(HttpHeader::Type::Age, "1234").setBody(body);
   request->setHeader(HttpHeader::Type::ContentLength, "1024").setCustomHeader("somename", "value");

   auto message = encode(request);

   ASSERT_TRUE(message != nullptr);
   ASSERT_EQ(2, message->getMajorVersion());
   ASSERT_EQ(1, message->getMinorVersion());

   request = std::dynamic_pointer_cast<http::HttpRequest>(message);

   ASSERT_TRUE(request != nullptr);
   ASSERT_TRUE(request->hasCustomHeader("somename"));
   ASSERT_EQ("value", request->getCustomHeaderValue("somename"));
   ASSERT_EQ(1024, request->getBody().size());
   ASSERT_TRUE(memcmp(request->getBody().data(), memory, 1024) == 0);
}

TEST(HttpProtocolDecoder, bad_response_two_items)
{
   basis::DataBlock dataBlock("HTTP/1.1 100 \r\n");
   protocol::HttpProtocolDecoder decoder;
   ASSERT_THROW(decoder.apply(dataBlock), basis::RuntimeException);
}


TEST(HttpProtocolDecoder, response_no_numeric_code)
{
   protocol::HttpProtocolDecoder decoder;

   {
      basis::DataBlock dataBlock("HTTP/1.1 123 some text\r\n");
      ASSERT_NO_THROW(decoder.apply(dataBlock));
   }

   {
      basis::DataBlock dataBlock("HTTP/1.1 abc some text\r\n");
      ASSERT_THROW(decoder.apply(dataBlock), basis::RuntimeException);
   }
}

TEST(HttpProtocolDecoder, bad_response_http)
{
   {
      basis::DataBlock dataBlock("HTTP1.1 100 aaaaa\r\n");
      protocol::HttpProtocolDecoder decoder;
      ASSERT_THROW(decoder.apply(dataBlock), basis::RuntimeException);
   }
   {
      basis::DataBlock dataBlock("HTTP/a.b 100 aaaaa\r\n");
      protocol::HttpProtocolDecoder decoder;
      ASSERT_THROW(decoder.apply(dataBlock), basis::RuntimeException);
   }
}

TEST(HttpProtocolDecoder, bad_request_http)
{
   {
      basis::DataBlock dataBlock("CONNECT the-uri HTTP1.1\r\n");
      protocol::HttpProtocolDecoder decoder;
      ASSERT_THROW(decoder.apply(dataBlock), basis::RuntimeException);
   }
   {
      basis::DataBlock dataBlock("CONNECT the-uri HTTP/a.b\r\n");
      protocol::HttpProtocolDecoder decoder;
      ASSERT_THROW(decoder.apply(dataBlock), basis::RuntimeException);
   }
}

TEST(HttpProtocolDecoder, request_bad_size ) {
   char memory[1024];
   basis::DataBlock body(memory, sizeof(memory));

   {
      auto request = HttpRequest::instantiate(HttpRequest::Method::Head, "/uri");
      request->setBody(body);
      request->setHeader(HttpHeader::Type::ContentLength, "1000");
      ASSERT_THROW(encode(request), basis::RuntimeException);
   }
   {
      auto request = HttpRequest::instantiate(HttpRequest::Method::Head, "/uri", 2);
      request->setBody(body);
      request->setHeader(HttpHeader::Type::ContentLength, "2048");
      ASSERT_THROW(encode(request), basis::RuntimeException);
   }
}
