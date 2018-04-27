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

#include <boost/test/unit_test.hpp>

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

BOOST_AUTO_TEST_CASE( http_protocol_decoder_basic_request )
{
   auto message = encode(HttpRequest::instantiate(HttpRequest::Method::Connect, "/uri/res"));

   BOOST_REQUIRE(message);
   BOOST_REQUIRE_EQUAL(message->getMajorVersion(), 1);
   BOOST_REQUIRE_EQUAL(message->getMinorVersion(), 1);

   {
      auto response = std::dynamic_pointer_cast<http::HttpResponse>(message);
      BOOST_REQUIRE(!response);
   }

   auto request = std::dynamic_pointer_cast<http::HttpRequest>(message);

   BOOST_REQUIRE(request);
   BOOST_REQUIRE_EQUAL(request->getMethod(), HttpRequest::Method::Connect);
   BOOST_REQUIRE_EQUAL(request->getPath(), "/uri/res");
   BOOST_REQUIRE(request->getBody().empty());
}

BOOST_AUTO_TEST_CASE( http_protocol_decoder_basic_response )
{
   auto response = HttpResponse::instantiate(HttpRequest::instantiate(HttpRequest::Method::Connect, "/uri"));
   response->setStatusCode(401);

   auto message = encode(response);

   BOOST_REQUIRE(message);
   BOOST_REQUIRE_EQUAL(message->getMajorVersion(), 1);
   BOOST_REQUIRE_EQUAL(message->getMinorVersion(), 1);

   {
      auto request = std::dynamic_pointer_cast<http::HttpRequest>(message);
      BOOST_REQUIRE(!request);
   }

   response = std::dynamic_pointer_cast<http::HttpResponse>(message);

   BOOST_REQUIRE(response);
   BOOST_REQUIRE_EQUAL(response->getStatusCode(), 401);
   BOOST_REQUIRE(response->getBody().empty());
}

BOOST_AUTO_TEST_CASE( http_protocol_decoder_header_request )
{
   auto request = HttpRequest::instantiate(HttpRequest::Method::Get, "/URI", 2);

   auto response = HttpResponse::instantiate(request);
   response->setStatusCode(200).setErrorDescription("Good!").setHeader(HttpHeader::Type::Age, "1234").setHeader(HttpHeader::Type::AcceptLanguage, "hava/x").setCustomHeader("MyHeader", "hello world");

   auto message = encode(response);

   BOOST_REQUIRE(message);
   BOOST_REQUIRE_EQUAL(message->getMajorVersion(), 2);
   BOOST_REQUIRE_EQUAL(message->getMinorVersion(), 1);

   response = std::dynamic_pointer_cast<http::HttpResponse>(message);

   BOOST_REQUIRE(response);
   BOOST_REQUIRE_EQUAL(response->getStatusCode(), 200);
   BOOST_REQUIRE(response->isOk());
   BOOST_REQUIRE_EQUAL(response->getErrorDescription(), "Good!");
   BOOST_REQUIRE(response->hasHeader(HttpHeader::Type::Age));
   BOOST_REQUIRE(response->hasCustomHeader("MyHeader"));
   BOOST_REQUIRE(!response->hasHeader(HttpHeader::Type::AcceptEncoding));
   BOOST_REQUIRE(!response->hasCustomHeader("OtherMyHeader"));
   BOOST_REQUIRE_EQUAL(response->getHeaderValue(HttpHeader::Type::Age), "1234");
   BOOST_REQUIRE_EQUAL(response->getHeaderValue(HttpHeader::Type::AcceptLanguage), "hava/x");
   BOOST_REQUIRE_EQUAL(response->getCustomHeaderValue("MyHeader"), "hello world");
   BOOST_REQUIRE(response->getBody().empty());
}

BOOST_AUTO_TEST_CASE( http_protocol_decoder_header_body_request )
{
   char memory[1024];
   basis::DataBlock body(memory,sizeof(memory));

   auto request = HttpRequest::instantiate(HttpRequest::Method::Head, "/uri/res", 2);
   request->setHeader(HttpHeader::Type::Age, "1234").setHeader(HttpHeader::Type::Age, "333").setBody(body);

   auto message = encode(request);

   BOOST_REQUIRE(message);
   BOOST_REQUIRE_EQUAL(message->getMajorVersion(), 2);
   BOOST_REQUIRE_EQUAL(message->getMinorVersion(), 1);

   request = std::dynamic_pointer_cast<http::HttpRequest>(message);

   BOOST_REQUIRE(request);
   BOOST_REQUIRE_EQUAL(request->getMethod(), HttpRequest::Method::Head);
   BOOST_REQUIRE_EQUAL(request->getPath(), "/uri/res");
   BOOST_REQUIRE_EQUAL(request->getHeaderValue(HttpHeader::Type::Age), "1234,333");
   BOOST_REQUIRE_EQUAL(request->getHeaderValue(HttpHeader::Type::ContentLength), "1024");
   BOOST_REQUIRE_EQUAL(request->getBody().size(), 1024);
   BOOST_REQUIRE(memcmp(request->getBody().data(), memory, 1024) == 0);
}

BOOST_AUTO_TEST_CASE( http_protocol_decoder_header_body_response )
{
   char memory[1024];
   basis::DataBlock body(memory,sizeof(memory));

   auto request = HttpRequest::instantiate(HttpRequest::Method::Head, "/path/resource", 2, 2);
   auto response = HttpResponse::instantiate(request);
   response->setStatusCode(1022).setCustomHeader("MyHeader", "1234").setCustomHeader("MyHeader", "333").setBody(body);

   auto message = encode(response);

   BOOST_REQUIRE(message);
   BOOST_REQUIRE_EQUAL(message->getMajorVersion(), 2);
   BOOST_REQUIRE_EQUAL(message->getMinorVersion(), 2);

   response = std::dynamic_pointer_cast<http::HttpResponse>(message);

   BOOST_REQUIRE(response);
   BOOST_REQUIRE_EQUAL(response->getStatusCode(), 1022);
   BOOST_REQUIRE_EQUAL(response->getErrorDescription(), "Unknown");
   BOOST_REQUIRE_EQUAL(response->getCustomHeaderValue("MyHeader"), "1234,333");
   BOOST_REQUIRE_EQUAL(response->getHeaderValue(HttpHeader::Type::ContentLength), "1024");
   BOOST_REQUIRE_EQUAL(response->getBody().size(), 1024);
   BOOST_REQUIRE(memcmp(response->getBody().data(), memory, 1024) == 0);
}

BOOST_AUTO_TEST_CASE( http_protocol_decoder_request_header_after_contentlength )
{
   char memory[1024];
   basis::DataBlock body(memory, sizeof(memory));

   auto request = HttpRequest::instantiate(HttpRequest::Method::Head, "/the:uri", 2);
   request->setHeader(HttpHeader::Type::Age, "1234").setBody(body);
   request->setHeader(HttpHeader::Type::ContentLength, "1024").setHeader(HttpHeader::Type::Allow, "allow");

   auto message = encode(request);

   BOOST_REQUIRE(message);
   BOOST_REQUIRE_EQUAL(message->getMajorVersion(), 2);
   BOOST_REQUIRE_EQUAL(message->getMinorVersion(), 1);

   request = std::dynamic_pointer_cast<http::HttpRequest>(message);

   BOOST_REQUIRE(request);
   BOOST_REQUIRE(request->hasHeader(HttpHeader::Type::Allow));
   BOOST_REQUIRE_EQUAL(request->getHeaderValue(HttpHeader::Type::Allow), "allow");
   BOOST_REQUIRE_EQUAL(request->getHeaderValue(HttpHeader::Type::ContentLength), "1024");
   BOOST_REQUIRE_EQUAL(request->getBody().size(), 1024);
   BOOST_REQUIRE(memcmp(request->getBody().data(), memory, 1024) == 0);
}

BOOST_AUTO_TEST_CASE( http_protocol_decoder_request_custom_header_after_contentlength )
{
   char memory[1024];
   basis::DataBlock body(memory, sizeof(memory));

   auto request = HttpRequest::instantiate(HttpRequest::Method::Head, "/uri", 2);
   request->setHeader(HttpHeader::Type::Age, "1234").setBody(body);
   request->setHeader(HttpHeader::Type::ContentLength, "1024").setCustomHeader("somename", "value");

   auto message = encode(request);

   BOOST_REQUIRE(message);
   BOOST_REQUIRE_EQUAL(message->getMajorVersion(), 2);
   BOOST_REQUIRE_EQUAL(message->getMinorVersion(), 1);

   request = std::dynamic_pointer_cast<http::HttpRequest>(message);

   BOOST_REQUIRE(request);
   BOOST_REQUIRE(request->hasCustomHeader("somename"));
   BOOST_REQUIRE_EQUAL(request->getCustomHeaderValue("somename"), "value");
   BOOST_REQUIRE_EQUAL(request->getBody().size(), 1024);
   BOOST_REQUIRE(memcmp(request->getBody().data(), memory, 1024) == 0);
}

BOOST_AUTO_TEST_CASE( http_protocol_decoder_bad_response_two_items)
{
   basis::DataBlock dataBlock("HTTP/1.1 100 \r\n");
   protocol::HttpProtocolDecoder decoder;
   BOOST_REQUIRE_THROW(decoder.apply(dataBlock), basis::RuntimeException);
}


BOOST_AUTO_TEST_CASE( http_protocol_decoder_response_no_numeric_code)
{
   protocol::HttpProtocolDecoder decoder;

   {
      basis::DataBlock dataBlock("HTTP/1.1 123 some text\r\n");
      BOOST_REQUIRE_NO_THROW(decoder.apply(dataBlock));
   }

   {
      basis::DataBlock dataBlock("HTTP/1.1 abc some text\r\n");
      BOOST_REQUIRE_THROW(decoder.apply(dataBlock), basis::RuntimeException);
   }
}

BOOST_AUTO_TEST_CASE( http_protocol_decoder_bad_response_http)
{
   {
      basis::DataBlock dataBlock("HTTP1.1 100 aaaaa\r\n");
      protocol::HttpProtocolDecoder decoder;
      BOOST_REQUIRE_THROW(decoder.apply(dataBlock), basis::RuntimeException);
   }
   {
      basis::DataBlock dataBlock("HTTP/a.b 100 aaaaa\r\n");
      protocol::HttpProtocolDecoder decoder;
      BOOST_REQUIRE_THROW(decoder.apply(dataBlock), basis::RuntimeException);
   }
}

BOOST_AUTO_TEST_CASE( http_protocol_decoder_bad_request_http)
{
   {
      basis::DataBlock dataBlock("CONNECT the-uri HTTP1.1\r\n");
      protocol::HttpProtocolDecoder decoder;
      BOOST_REQUIRE_THROW(decoder.apply(dataBlock), basis::RuntimeException);
   }
   {
      basis::DataBlock dataBlock("CONNECT the-uri HTTP/a.b\r\n");
      protocol::HttpProtocolDecoder decoder;
      BOOST_REQUIRE_THROW(decoder.apply(dataBlock), basis::RuntimeException);
   }
}

BOOST_AUTO_TEST_CASE( http_protocol_decoder_request_bad_size ) {
   char memory[1024];
   basis::DataBlock body(memory, sizeof(memory));

   {
      auto request = HttpRequest::instantiate(HttpRequest::Method::Head, "/uri");
      request->setBody(body);
      request->setHeader(HttpHeader::Type::ContentLength, "1000");
      BOOST_REQUIRE_THROW(encode(request), basis::RuntimeException);
   }
   {
      auto request = HttpRequest::instantiate(HttpRequest::Method::Head, "/uri", 2);
      request->setBody(body);
      request->setHeader(HttpHeader::Type::ContentLength, "2048");
      BOOST_REQUIRE_THROW(encode(request), basis::RuntimeException);
   }
}
