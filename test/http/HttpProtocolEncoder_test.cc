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

struct HttpTestSplitter {
   HttpTestSplitter(const basis::DataBlock& dataBlock) : m_dataBlock(dataBlock) {;}
   const std::string& readLine() throw (basis::RuntimeException);
   const basis::DataBlock& readDataBlock() throw (basis::RuntimeException);
   protocol::Token token;
   const basis::DataBlock& m_dataBlock;
};

const std::string& HttpTestSplitter::readLine()
   throw (basis::RuntimeException)
{
   if (protocol::HttpProtocolDecoder::readToken(m_dataBlock, token))
      return token.value;

   COFFEE_THROW_EXCEPTION("No read token");
}

const basis::DataBlock&  HttpTestSplitter::readDataBlock()
   throw (basis::RuntimeException)
{
   if (protocol::HttpProtocolDecoder::readToken(m_dataBlock, token))
      return token.value;

   COFFEE_THROW_EXCEPTION("No read token");
}

BOOST_AUTO_TEST_CASE( http_protocol_encoder_basic_request )
{
   auto request = HttpRequest::instantiate(HttpRequest::Method::Connect, "/uri/com");

   protocol::HttpProtocolEncoder encoder;

   HttpTestSplitter splitter(encoder.apply(request));

   BOOST_REQUIRE_EQUAL(splitter.readLine(), "CONNECT /uri/com HTTP/1.1");
   BOOST_REQUIRE(splitter.readLine().empty());
}

BOOST_AUTO_TEST_CASE( http_protocol_encoder_basic_response )
{
   auto request = HttpRequest::instantiate(HttpRequest::Method::Connect, "/resource/id");
   auto response = HttpResponse::instantiate(request);
   response->setStatusCode(401);

   protocol::HttpProtocolEncoder encoder;

   HttpTestSplitter splitter(encoder.apply(response));

   BOOST_REQUIRE_EQUAL(splitter.readLine(), "HTTP/1.1 401 Unautorized");
   BOOST_REQUIRE(splitter.readLine().empty());
}

BOOST_AUTO_TEST_CASE( http_protocol_encoder_header_request )
{
   auto request = HttpRequest::instantiate(HttpRequest::Method::Get, "/path/resource", 2);

   auto response = HttpResponse::instantiate(request);
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

   auto request = HttpRequest::instantiate(HttpRequest::Method::Head, "/path/resource", 2);
   request->setHeader(HttpHeader::Type::Age, "1234").setHeader(HttpHeader::Type::Age, "333").setBody(body);

   protocol::HttpProtocolEncoder encoder;
   const basis::DataBlock& encode = encoder.apply(request);
   HttpTestSplitter splitter(encode);

   BOOST_REQUIRE_EQUAL(splitter.readLine(), "HEAD /path/resource HTTP/2.1");
   BOOST_REQUIRE_EQUAL(splitter.readLine(), "Age:1234,333");
   BOOST_REQUIRE_EQUAL(splitter.readLine(), "Content-Length:1024");
   BOOST_REQUIRE(splitter.readLine().empty());

   const basis::DataBlock& readBody = splitter.readDataBlock();

   BOOST_REQUIRE_EQUAL(readBody.size(), 1024);
   BOOST_REQUIRE(memcmp(readBody.data(), memory, 1024) == 0);
}

BOOST_AUTO_TEST_CASE( http_protocol_encoder_header_body_response )
{
   char memory[1024];
   basis::DataBlock body(memory,sizeof(memory));

   auto request = HttpRequest::instantiate(HttpRequest::Method::Head, "/uri/res", 2, 2);

   auto response = HttpResponse::instantiate(request);
   response->setStatusCode(999).setErrorDescription("This is my description");
   response->setHeader(HttpHeader::Type::Age, "1234").setHeader(HttpHeader::Type::Age, "333").setBody(body);

   protocol::HttpProtocolEncoder encoder;
   const basis::DataBlock& encode = encoder.apply(response);
   HttpTestSplitter splitter(encode);

   BOOST_REQUIRE_EQUAL(splitter.readLine(), "HTTP/2.2 999 This is my description");
   BOOST_REQUIRE_EQUAL(splitter.readLine(), "Age:1234,333");
   BOOST_REQUIRE_EQUAL(splitter.readLine(), "Content-Length:1024");
   BOOST_REQUIRE(splitter.readLine().empty());

   const basis::DataBlock& readBody = splitter.readDataBlock();

   BOOST_REQUIRE_EQUAL(readBody.size(), 1024);
   BOOST_REQUIRE(memcmp(readBody.data(), memory, 1024) == 0);
}
