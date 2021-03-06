// MIT License
//
// Copyright(c) 2018 Francisco Ruiz(francisco.ruiz.rayo@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
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

#include <coffee/http/HttpCustomHeader.hpp>
#include <coffee/http/HttpHeader.hpp>
#include <coffee/http/HttpMessage.hpp>
#include <coffee/http/HttpRequest.hpp>

using namespace coffee;

using coffee::http::HttpMessage;
using coffee::http::HttpHeader;

TEST(HttpMessageTest, has_header )
{
   auto message = http::HttpRequest::instantiate(http::HttpRequest::Method::Delete, "/uri");

   ASSERT_TRUE(!message->hasHeader(HttpHeader::Type::Host));
   message->setHeader(HttpHeader::Type::Host, "localhost");
   ASSERT_TRUE(message->hasHeader(HttpHeader::Type::Host));
}

TEST(HttpMessageTest, has_header_bad_custom )
{
   auto message = http::HttpRequest::instantiate(http::HttpRequest::Method::Delete, "/uri");

   ASSERT_THROW(message->hasHeader(HttpHeader::Type::Custom), basis::RuntimeException);
}

TEST(HttpMessageTest, set_header_bad_custom )
{
   auto message = http::HttpRequest::instantiate(http::HttpRequest::Method::Delete, "/uri");

   ASSERT_THROW(message->setHeader(HttpHeader::Type::Custom, "somevalue"), basis::RuntimeException);
}

TEST(HttpMessageTest, get_header_bad_custom )
{
   auto message = http::HttpRequest::instantiate(http::HttpRequest::Method::Delete, "/uri");

   ASSERT_THROW(message->getHeaderValue(HttpHeader::Type::Custom), basis::RuntimeException);
}

TEST(HttpMessageTest, get_custom_header )
{
   auto message = http::HttpRequest::instantiate(http::HttpRequest::Method::Delete, "/uri");

   ASSERT_TRUE(!message->hasCustomHeader("SomeName"));
   ASSERT_THROW(message->getCustomHeaderValue("SomeName"), basis::RuntimeException);
   message->setCustomHeader("SomeName", "localhost");
   ASSERT_EQ("localhost", message->getCustomHeaderValue("SomeName"));
}
