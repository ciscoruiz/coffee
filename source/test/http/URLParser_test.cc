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

#include <coffee/http/url/URLParser.hpp>
#include <coffee/http/url/URL.hpp>

using namespace coffee;

using coffee::http::url::URL;
using http::url::ComponentName;

TEST(HttpUrlParserTest, set_all_components)
{
   const char* expression = "tcp://user:pwd@localhost.me:8080/path/resource?key1=value1&key2=value2#fragment";

   http::url::URLParser parser(expression);

   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      ASSERT_TRUE(url->hasComponent((ComponentName::_v) ii));
   }

   ASSERT_EQ("tcp", url->getComponent(ComponentName::Scheme));
   ASSERT_EQ("user", url->getComponent(ComponentName::User));
   ASSERT_EQ("pwd", url->getComponent(ComponentName::Password));
   ASSERT_EQ("localhost.me", url->getComponent(ComponentName::Host));
   ASSERT_EQ("8080", url->getComponent(ComponentName::Port));
   ASSERT_EQ("/path/resource", url->getComponent(ComponentName::Path));
   ASSERT_EQ("#fragment", url->getComponent(ComponentName::Fragment));

   auto ii = url->query_begin();
   auto keyValue = URL::keyValue(ii);
   ASSERT_EQ("key1", keyValue.first);
   ASSERT_EQ("value1", keyValue.second);

   keyValue = URL::keyValue(++ ii);
   ASSERT_EQ("key2", keyValue.first);
   ASSERT_EQ("value2", keyValue.second);

   ASSERT_TRUE(++ ii == url->query_end());

   ASSERT_EQ(expression, url->encode());
}

TEST(HttpUrlParserTest, short_url)
{
   const bool expected[] = { false, false, false, false, false, true, true};

   const char* expression = "/path/resource?key1=value1&key2=value2#fragment";

   http::url::URLParser parser(expression);

   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      ASSERT_TRUE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   ASSERT_EQ("/path/resource", url->getComponent(ComponentName::Path));
   ASSERT_EQ("#fragment", url->getComponent(ComponentName::Fragment));

   auto ii = url->query_begin();
   auto keyValue = URL::keyValue(ii);
   ASSERT_EQ("key1", keyValue.first);
   ASSERT_EQ("value1", keyValue.second);

   keyValue = URL::keyValue(++ ii);
   ASSERT_EQ("key2", keyValue.first);
   ASSERT_EQ("value2", keyValue.second);

   ASSERT_TRUE(++ ii == url->query_end());

   ASSERT_EQ(expression, url->encode());
}

TEST(HttpUrlParserTest, minimal)
{
   const bool expected[] = { true, false, false, true, false, false, false };

   http::url::URLParser parser("tcp:localhost.me");
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      ASSERT_TRUE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   ASSERT_EQ("tcp", url->getComponent(ComponentName::Scheme));
   ASSERT_EQ("localhost.me", url->getComponent(ComponentName::Host));
   ASSERT_TRUE(!url->hasQuery());

   ASSERT_EQ("tcp://localhost.me", url->encode());
}

TEST(HttpUrlParserTest, minimal_slash)
{
   const bool expected[] = { true, false, false, true, false, false, false };

   http::url::URLParser parser("tcp://localhost.me");
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      ASSERT_TRUE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   ASSERT_EQ("tcp", url->getComponent(ComponentName::Scheme));
   ASSERT_EQ("localhost.me", url->getComponent(ComponentName::Host));
   ASSERT_TRUE(!url->hasQuery());

   ASSERT_EQ("tcp://localhost.me", url->encode());
}

TEST(HttpUrlParserTest, minimal_port)
{
   const bool expected[] = { true, false, false, true, true, false, false };
   const char* expression = "tcp://localhost.me:8080";

   http::url::URLParser parser(expression);
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      ASSERT_TRUE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   ASSERT_EQ("tcp", url->getComponent(ComponentName::Scheme));
   ASSERT_EQ("localhost.me", url->getComponent(ComponentName::Host));
   ASSERT_EQ("8080", url->getComponent(ComponentName::Port));
   ASSERT_TRUE(!url->hasQuery());

   ASSERT_EQ(expression, url->encode());
}

TEST(HttpUrlParserTest, minimal_path)
{
   const bool expected[] = { true, false, false, true, false, true, false };
   const char* expression = "tcp://localhost.me/path/resource";

   http::url::URLParser parser(expression);
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      ASSERT_TRUE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   ASSERT_EQ("tcp", url->getComponent(ComponentName::Scheme));
   ASSERT_EQ("localhost.me", url->getComponent(ComponentName::Host));
   ASSERT_EQ("/path/resource", url->getComponent(ComponentName::Path));
   ASSERT_TRUE(!url->hasQuery());

   ASSERT_EQ(expression, url->encode());
}

TEST(HttpUrlParserTest, minimal_query)
{
   const bool expected[] = { true, false, false, true, true, false, false };
   const char* expression = "tcp://localhost.me:8080?key1=value1&key2=value2";

   http::url::URLParser parser(expression);
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      ASSERT_TRUE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   ASSERT_EQ("tcp", url->getComponent(ComponentName::Scheme));
   ASSERT_EQ("localhost.me", url->getComponent(ComponentName::Host));
   ASSERT_EQ("8080", url->getComponent(ComponentName::Port));
   ASSERT_TRUE(url->hasQuery());

   auto ii = url->query_begin();
   auto keyValue = URL::keyValue(ii);
   ASSERT_EQ("key1", keyValue.first);
   ASSERT_EQ("value1", keyValue.second);

   ASSERT_EQ(expression, url->encode());
}

TEST(HttpUrlParserTest, minimal_query_empty_value)
{
   const bool expected[] = { true, false, false, true, true, false, false };
   const char* expression = "tcp://localhost.me:8080?key1";

   http::url::URLParser parser(expression);
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      ASSERT_TRUE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   ASSERT_EQ("tcp", url->getComponent(ComponentName::Scheme));
   ASSERT_EQ("localhost.me", url->getComponent(ComponentName::Host));
   ASSERT_EQ("8080", url->getComponent(ComponentName::Port));
   ASSERT_TRUE(url->hasQuery());

   auto ii = url->query_begin();
   auto keyValue = URL::keyValue(ii);
   ASSERT_EQ("key1", keyValue.first);
   ASSERT_TRUE(keyValue.second.empty());

   ASSERT_EQ(expression, url->encode());
}

TEST(HttpUrlParserTest, minimal_fragment)
{
   const bool expected[] = { true, false, false, true, false, false, true };
   const char* expression = "tcp://localhost.me#reference";

   http::url::URLParser parser(expression);
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      ASSERT_TRUE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   ASSERT_EQ("tcp", url->getComponent(ComponentName::Scheme));
   ASSERT_EQ("localhost.me", url->getComponent(ComponentName::Host));
   ASSERT_EQ("#reference", url->getComponent(ComponentName::Fragment));
   ASSERT_TRUE(!url->hasQuery());

   ASSERT_EQ(expression, url->encode());
}

TEST(HttpUrlParserTest, user)
{
   const bool expected[] = { true, true, false, true, false, false, true };
   const char* expression = "tcp://user@localhost.me#reference";

   http::url::URLParser parser(expression);
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      ASSERT_TRUE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   ASSERT_EQ("tcp", url->getComponent(ComponentName::Scheme));
   ASSERT_EQ("user", url->getComponent(ComponentName::User));
   ASSERT_EQ("localhost.me", url->getComponent(ComponentName::Host));
   ASSERT_EQ("#reference", url->getComponent(ComponentName::Fragment));
   ASSERT_TRUE(!url->hasQuery());

   ASSERT_EQ(expression, url->encode());
}

TEST(HttpUrlParserTest, pwd)
{
   const bool expected[] = { true, false, true, true, false, false, false };
   const char* expression = "tcp://:pwd@localhost.me";

   http::url::URLParser parser(expression);
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      ASSERT_TRUE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   ASSERT_EQ("tcp", url->getComponent(ComponentName::Scheme));
   ASSERT_EQ("pwd", url->getComponent(ComponentName::Password));
   ASSERT_EQ("localhost.me", url->getComponent(ComponentName::Host));
   ASSERT_TRUE(!url->hasQuery());

   ASSERT_EQ(expression, url->encode());
}

TEST(HttpUrlParserTest, user_pwd)
{
   const bool expected[] = { true, true, true, true, false, false, false };
   const char* expression = "sctp://one_guy:pwd@localhost.me";

   http::url::URLParser parser(expression);
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      ASSERT_TRUE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   ASSERT_EQ("sctp", url->getComponent(ComponentName::Scheme));
   ASSERT_EQ("one_guy", url->getComponent(ComponentName::User));
   ASSERT_EQ("pwd", url->getComponent(ComponentName::Password));
   ASSERT_EQ("localhost.me", url->getComponent(ComponentName::Host));
   ASSERT_TRUE(!url->hasQuery());

   ASSERT_EQ(expression, url->encode());
}

TEST(HttpUrlParserTest, empty_path)
{
   const bool expected[] = { true, false, false, true, false, true, false };
   const char* expression = "tcp://localhost.me/";

   http::url::URLParser parser(expression);
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      ASSERT_TRUE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   ASSERT_EQ("tcp", url->getComponent(ComponentName::Scheme));
   ASSERT_EQ("localhost.me", url->getComponent(ComponentName::Host));
   ASSERT_EQ("/", url->getComponent(ComponentName::Path));
   ASSERT_TRUE(!url->hasQuery());

   ASSERT_EQ(expression, url->encode());
}

TEST(HttpUrlParserTest, no_scheme)
{
   http::url::URLParser parser("localhost.me/");
   ASSERT_THROW(parser.build(), basis::RuntimeException);
}

TEST(HttpUrlParserTest, empty_scheme)
{
   http::url::URLParser parser(":localhost.me");
   ASSERT_THROW(parser.build(), basis::RuntimeException);
}

TEST(HttpUrlParserTest, no_host)
{
   http::url::URLParser parser("tcp://:8080");
   ASSERT_THROW(parser.build(), basis::RuntimeException);
}

TEST(HttpUrlParserTest, bad_query)
{
   http::url::URLParser parser("http://localhost.me?=zzz");
   ASSERT_THROW(parser.build(), basis::RuntimeException);
}

TEST(HttpUrlParserTest, no_numeric_port)
{
   http::url::URLParser parser("tcp://host.com:123a");
   ASSERT_THROW(parser.build(), basis::RuntimeException);
}
