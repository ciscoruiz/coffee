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
#include <coffee/http/protocol/HttpProtocolEncoder.hpp>
#include <coffee/http/HttpRequest.hpp>
#include <coffee/http/HttpResponse.hpp>

using namespace coffee;
using namespace coffee::http;

struct HttpTestSplitter {
   HttpTestSplitter(const basis::DataBlock& dataBlock) :
      m_dataBlock(dataBlock), pos(0), cursorPos(0), maxPos(dataBlock.size())
   {;}

   std::string readLine() noexcept;

   const basis::DataBlock& m_dataBlock;
   std::string::size_type cursorPos, pos;
   std::string::size_type maxPos;
};

std::string HttpTestSplitter::readLine()
   noexcept
{
   const int nchars = coffee_strlen(http::protocol::HttpProtocolEncoder::newLineCharacters);

   pos = m_dataBlock.find(http::protocol::HttpProtocolEncoder::newLineCharacters, cursorPos);

   if (pos != std::string::npos) {
      if (pos == cursorPos) {
         cursorPos += nchars;
         return std::string();
      }
      std::string result = m_dataBlock.substr(cursorPos, pos - cursorPos);
      cursorPos = pos + nchars;
      return result;
   }
   else {
      std::string result = m_dataBlock.substr(cursorPos);
      return result;
   }
}

BOOST_AUTO_TEST_CASE( http_protocol_encoder_basic_request )
{
   url::URLParser parser("http:uri.com");

   auto request = HttpRequest::instantiate(HttpRequest::Method::Connect, parser.build());

   protocol::HttpProtocolEncoder encoder;

   HttpTestSplitter splitter(encoder.apply(request));

   BOOST_REQUIRE_EQUAL(splitter.readLine(), "CONNECT http://uri.com HTTP/1.1");
   BOOST_REQUIRE(splitter.readLine().empty());
}

BOOST_AUTO_TEST_CASE( http_protocol_encoder_basic_response )
{
   url::URLParser parser("http:uri.com");

   auto request = HttpRequest::instantiate(HttpRequest::Method::Connect, parser.build());
   auto response = HttpResponse::instantiate(401, request);

   protocol::HttpProtocolEncoder encoder;

   HttpTestSplitter splitter(encoder.apply(response));

   BOOST_REQUIRE_EQUAL(splitter.readLine(), "HTTP/1.1 401 Unautorized");
   BOOST_REQUIRE(splitter.readLine().empty());
}

BOOST_AUTO_TEST_CASE( http_protocol_encoder_header_request )
{
   url::URLParser parser("http:user@uri.com");

   auto request = HttpRequest::instantiate(HttpRequest::Method::Get, parser.build(), 2);

   auto response = HttpResponse::instantiate(200, request);
   response->setHeader(HttpHeader::Type::Age, "1234").setHeader(HttpHeader::Type::AcceptLanguage, "hava/x");

   protocol::HttpProtocolEncoder encoder;

   HttpTestSplitter splitter(encoder.apply(response));

   BOOST_REQUIRE_EQUAL(splitter.readLine(), "HTTP/2.1 200 OK");
   BOOST_REQUIRE_EQUAL(splitter.readLine(), "Age:1234");
   BOOST_REQUIRE_EQUAL(splitter.readLine(), "Accept-Language:hava/x");
   BOOST_REQUIRE(splitter.readLine().empty());
}

BOOST_AUTO_TEST_CASE( http_protocol_encoder_header_body_request )
{
   char memory[1024];
   basis::DataBlock body(memory,sizeof(memory));

   url::URLParser parser("http:user:password@uri.com");
   auto request = HttpRequest::instantiate(HttpRequest::Method::Head, parser.build(), 2);
   request->setHeader(HttpHeader::Type::Age, "1234").setHeader(HttpHeader::Type::Age, "333").setBody(body);

   protocol::HttpProtocolEncoder encoder;
   const basis::DataBlock& encode = encoder.apply(request);
   HttpTestSplitter splitter(encode);

   BOOST_REQUIRE_EQUAL(splitter.readLine(), "HEAD http://user:password@uri.com HTTP/2.1");
   BOOST_REQUIRE_EQUAL(splitter.readLine(), "Age:1234,333");
   BOOST_REQUIRE_EQUAL(splitter.readLine(), "Content-Length:1024");
   BOOST_REQUIRE(splitter.readLine().empty());

   BOOST_REQUIRE_EQUAL(encode.size() - splitter.cursorPos, 1024);

   basis::DataBlock encodeBody(encode.data() + splitter.cursorPos, 1024);
   BOOST_REQUIRE(memcmp(encodeBody.data(), memory, 1024) == 0);
}

BOOST_AUTO_TEST_CASE( http_protocol_encoder_header_body_response )
{
   char memory[1024];
   basis::DataBlock body(memory,sizeof(memory));

   url::URLParser parser("http:user:password@uri.com");
   auto request = HttpRequest::instantiate(HttpRequest::Method::Head, parser.build(), 2, 2);

   auto response = HttpResponse::instantiate(999, request);
   response->setHeader(HttpHeader::Type::Age, "1234").setHeader(HttpHeader::Type::Age, "333").setBody(body);

   protocol::HttpProtocolEncoder encoder;
   const basis::DataBlock& encode = encoder.apply(response);
   HttpTestSplitter splitter(encode);

   BOOST_REQUIRE_EQUAL(splitter.readLine(), "HTTP/2.2 999 Unknown");
   BOOST_REQUIRE_EQUAL(splitter.readLine(), "Age:1234,333");
   BOOST_REQUIRE_EQUAL(splitter.readLine(), "Content-Length:1024");
   BOOST_REQUIRE(splitter.readLine().empty());

   BOOST_REQUIRE_EQUAL(encode.size() - splitter.cursorPos, 1024);

   basis::DataBlock encodeBody(encode.data() + splitter.cursorPos, 1024);
   BOOST_REQUIRE(memcmp(encodeBody.data(), memory, 1024) == 0);
}
