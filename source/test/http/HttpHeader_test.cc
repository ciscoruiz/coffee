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

#include <coffee/http/HttpHeader.hpp>
#include <coffee/http/HttpCustomHeader.hpp>

using namespace coffee;

using coffee::http::HttpHeader;

TEST(HttpHeaderTest, predefined_encode)
{
   HttpHeader header(HttpHeader::Type::LastModified);
   header.setValue("my value");
   ASSERT_EQ("Last-Modified:my value", header.encode());
}

TEST(HttpHeaderTest, instantiate_encode)
{
   auto header = HttpHeader::instantiate(HttpHeader::Type::AcceptLanguage);
   header->setValue("havax");
   basis::StreamString ss(HttpHeader::Type::asString(HttpHeader::Type::AcceptLanguage));
   ASSERT_EQ(ss << ":havax", header->encode());
}

TEST(HttpHeaderTest, exception_encode)
{
   HttpHeader header(http::HttpHeader::Type::Custom);
   header.setValue("my value");
   ASSERT_THROW(header.encode(), basis::RuntimeException);
}

TEST(HttpHeaderTest, custom_encode)
{
   auto header = http::HttpCustomHeader::instantiate("MyCustomHeader");
   ASSERT_TRUE(header->getType() == HttpHeader::Type::Custom);
   header->setValue("my value");
   ASSERT_EQ("MyCustomHeader:my value", header->encode());
}

TEST(HttpHeaderTest, multivalue)
{
   auto header = http::HttpCustomHeader::instantiate("MyCustomHeader");
   ASSERT_TRUE(header->getType() == HttpHeader::Type::Custom);
   header->setValue(123);
   header->setValue("after");
   ASSERT_EQ("MyCustomHeader:123,after", header->encode());
}
