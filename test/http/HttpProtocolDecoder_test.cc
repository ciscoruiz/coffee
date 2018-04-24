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

#include <coffee/http/url/URLParser.hpp>
#include <coffee/http/url/URL.hpp>
#include <coffee/http/protocol/HttpProtocolEncoder.hpp>
#include <coffee/http/HttpRequest.hpp>
#include <coffee/http/HttpResponse.hpp>
#include <coffee/http/protocol/defines.hpp>
#include <coffee/http/protocol/HttpProtocolDecoder.hpp>

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
   url::URLParser parser("http:uri.com");
   auto message = encode(HttpRequest::instantiate(HttpRequest::Method::Connect, parser.build()));

   BOOST_REQUIRE(message);
   BOOST_REQUIRE_EQUAL(message->getMajorVersion(), 1);
   BOOST_REQUIRE_EQUAL(message->getMinorVersion(), 1);

   auto request = std::dynamic_pointer_cast<http::HttpRequest>(message);

   BOOST_REQUIRE(request);
   BOOST_REQUIRE_EQUAL(request->getMethod(), HttpRequest::Method::Connect);
   BOOST_REQUIRE_EQUAL(request->getURL()->encode(), "http://uri.com");
   BOOST_REQUIRE(request->getBody().empty());
}

BOOST_AUTO_TEST_CASE( http_protocol_decoder_basic_response )
{
   url::URLParser parser("http:uri.com");

   auto message = encode(HttpResponse::instantiate(401, HttpRequest::instantiate(HttpRequest::Method::Connect, parser.build())));

   BOOST_REQUIRE(message);
   BOOST_REQUIRE_EQUAL(message->getMajorVersion(), 1);
   BOOST_REQUIRE_EQUAL(message->getMinorVersion(), 1);

   auto response = std::dynamic_pointer_cast<http::HttpResponse>(message);

   BOOST_REQUIRE(response);
   BOOST_REQUIRE_EQUAL(response->getStatusCode(), 401);
   BOOST_REQUIRE(response->getBody().empty());
}

BOOST_AUTO_TEST_CASE( http_protocol_decoder_header_request )
{
   url::URLParser parser("http:user@uri.com");

   auto request = HttpRequest::instantiate(HttpRequest::Method::Get, parser.build(), 2);

   auto response = HttpResponse::instantiate(200, request);
   response->setHeader(HttpHeader::Type::Age, "1234").setHeader(HttpHeader::Type::AcceptLanguage, "hava/x").setCustomHeader("MyHeader", "hello world");

   auto message = encode(response);

   BOOST_REQUIRE(message);
   BOOST_REQUIRE_EQUAL(message->getMajorVersion(), 2);
   BOOST_REQUIRE_EQUAL(message->getMinorVersion(), 1);

   response = std::dynamic_pointer_cast<http::HttpResponse>(message);

   BOOST_REQUIRE_EQUAL(response->getStatusCode(), 200);
   BOOST_REQUIRE_EQUAL(response->getHeaderValue(HttpHeader::Type::Age), "1234");
   BOOST_REQUIRE_EQUAL(response->getHeaderValue(HttpHeader::Type::AcceptLanguage), "hava/x");
   BOOST_REQUIRE_EQUAL(response->getCustomHeaderValue("MyHeader"), "hello world");
   BOOST_REQUIRE(response->getBody().empty());
}

BOOST_AUTO_TEST_CASE( http_protocol_decoder_header_body_request )
{
   char memory[1024];
   basis::DataBlock body(memory,sizeof(memory));

   url::URLParser parser("http:user:password@uri.com");
   auto request = HttpRequest::instantiate(HttpRequest::Method::Head, parser.build(), 2);
   request->setHeader(HttpHeader::Type::Age, "1234").setHeader(HttpHeader::Type::Age, "333").setBody(body);

   auto message = encode(request);

   BOOST_REQUIRE(message);
   BOOST_REQUIRE_EQUAL(message->getMajorVersion(), 2);
   BOOST_REQUIRE_EQUAL(message->getMinorVersion(), 1);

   request = std::dynamic_pointer_cast<http::HttpRequest>(message);

   BOOST_REQUIRE(request);
   BOOST_REQUIRE_EQUAL(request->getMethod(), HttpRequest::Method::Head);
   BOOST_REQUIRE_EQUAL(request->getURL()->encode(), "http://user:password@uri.com");
   BOOST_REQUIRE_EQUAL(request->getHeaderValue(HttpHeader::Type::Age), "1234,333");
   BOOST_REQUIRE_EQUAL(request->getHeaderValue(HttpHeader::Type::ContentLength), "1024");
   BOOST_REQUIRE_EQUAL(request->getBody().size(), 1024);
   BOOST_REQUIRE(memcmp(request->getBody().data(), memory, 1024) == 0);
}

BOOST_AUTO_TEST_CASE( http_protocol_decoder_header_body_response )
{
   char memory[1024];
   basis::DataBlock body(memory,sizeof(memory));

   url::URLParser parser("http:user:password@uri.com");
   auto request = HttpRequest::instantiate(HttpRequest::Method::Head, parser.build(), 2, 2);
   auto response = HttpResponse::instantiate(999, request);
   response->setCustomHeader("MyHeader", "1234").setCustomHeader("MyHeader", "333").setBody(body);

   auto message = encode(response);

   BOOST_REQUIRE(message);
   BOOST_REQUIRE_EQUAL(message->getMajorVersion(), 2);
   BOOST_REQUIRE_EQUAL(message->getMinorVersion(), 2);

   response = std::dynamic_pointer_cast<http::HttpResponse>(message);

   BOOST_REQUIRE_EQUAL(response->getStatusCode(), 999);
   BOOST_REQUIRE_EQUAL(response->getCustomHeaderValue("MyHeader"), "1234,333");
   BOOST_REQUIRE_EQUAL(response->getHeaderValue(HttpHeader::Type::ContentLength), "1024");
   BOOST_REQUIRE_EQUAL(response->getBody().size(), 1024);
   BOOST_REQUIRE(memcmp(response->getBody().data(), memory, 1024) == 0);
}
