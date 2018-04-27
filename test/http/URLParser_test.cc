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

using namespace coffee;

using coffee::http::url::URL;
using http::url::ComponentName;

BOOST_AUTO_TEST_CASE(URLParser_set_all_components)
{
   const char* expression = "tcp://user:pwd@localhost.me:8080/path/resource?key1=value1&key2=value2#fragment";

   http::url::URLParser parser(expression);

   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      BOOST_REQUIRE(url->hasComponent((ComponentName::_v) ii));
   }

   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Scheme), "tcp");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::User), "user");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Password), "pwd");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Host), "localhost.me");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Port), "8080");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Path), "/path/resource");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Fragment), "#fragment");

   auto ii = url->query_begin();
   auto keyValue = URL::keyValue(ii);
   BOOST_REQUIRE_EQUAL(keyValue.first, "key1");
   BOOST_REQUIRE_EQUAL(keyValue.second, "value1");

   keyValue = URL::keyValue(++ ii);
   BOOST_REQUIRE_EQUAL(keyValue.first, "key2");
   BOOST_REQUIRE_EQUAL(keyValue.second, "value2");

   BOOST_REQUIRE(++ ii == url->query_end());

   BOOST_REQUIRE_EQUAL(url->encode(), expression);
}

BOOST_AUTO_TEST_CASE(URLParser_short_url)
{
   const bool expected[] = { false, false, false, false, false, true, true};

   const char* expression = "/path/resource?key1=value1&key2=value2#fragment";

   http::url::URLParser parser(expression);

   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      BOOST_REQUIRE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Path), "/path/resource");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Fragment), "#fragment");

   auto ii = url->query_begin();
   auto keyValue = URL::keyValue(ii);
   BOOST_REQUIRE_EQUAL(keyValue.first, "key1");
   BOOST_REQUIRE_EQUAL(keyValue.second, "value1");

   keyValue = URL::keyValue(++ ii);
   BOOST_REQUIRE_EQUAL(keyValue.first, "key2");
   BOOST_REQUIRE_EQUAL(keyValue.second, "value2");

   BOOST_REQUIRE(++ ii == url->query_end());

   BOOST_REQUIRE_EQUAL(url->encode(), expression);
}

BOOST_AUTO_TEST_CASE(URLParser_minimal)
{
   const bool expected[] = { true, false, false, true, false, false, false };

   http::url::URLParser parser("tcp:localhost.me");
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      BOOST_REQUIRE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Scheme), "tcp");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Host), "localhost.me");
   BOOST_REQUIRE(!url->hasQuery());

   BOOST_REQUIRE_EQUAL(url->encode(), "tcp://localhost.me");
}

BOOST_AUTO_TEST_CASE(URLParser_minimal_slash)
{
   const bool expected[] = { true, false, false, true, false, false, false };

   http::url::URLParser parser("tcp://localhost.me");
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      BOOST_REQUIRE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Scheme), "tcp");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Host), "localhost.me");
   BOOST_REQUIRE(!url->hasQuery());

   BOOST_REQUIRE_EQUAL(url->encode(), "tcp://localhost.me");
}

BOOST_AUTO_TEST_CASE(URLParser_minimal_port)
{
   const bool expected[] = { true, false, false, true, true, false, false };
   const char* expression = "tcp://localhost.me:8080";

   http::url::URLParser parser(expression);
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      BOOST_REQUIRE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Scheme), "tcp");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Host), "localhost.me");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Port), "8080");
   BOOST_REQUIRE(!url->hasQuery());

   BOOST_REQUIRE_EQUAL(url->encode(), expression);
}

BOOST_AUTO_TEST_CASE(URLParser_minimal_path)
{
   const bool expected[] = { true, false, false, true, false, true, false };
   const char* expression = "tcp://localhost.me/path/resource";

   http::url::URLParser parser(expression);
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      BOOST_REQUIRE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Scheme), "tcp");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Host), "localhost.me");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Path), "/path/resource");
   BOOST_REQUIRE(!url->hasQuery());

   BOOST_REQUIRE_EQUAL(url->encode(), expression);
}

BOOST_AUTO_TEST_CASE(URLParser_minimal_query)
{
   const bool expected[] = { true, false, false, true, true, false, false };
   const char* expression = "tcp://localhost.me:8080?key1=value1&key2=value2";

   http::url::URLParser parser(expression);
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      BOOST_REQUIRE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Scheme), "tcp");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Host), "localhost.me");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Port), "8080");
   BOOST_REQUIRE(url->hasQuery());

   auto ii = url->query_begin();
   auto keyValue = URL::keyValue(ii);
   BOOST_REQUIRE_EQUAL(keyValue.first, "key1");
   BOOST_REQUIRE_EQUAL(keyValue.second, "value1");

   BOOST_REQUIRE_EQUAL(url->encode(), expression);
}

BOOST_AUTO_TEST_CASE(URLParser_minimal_query_empty_value)
{
   const bool expected[] = { true, false, false, true, true, false, false };
   const char* expression = "tcp://localhost.me:8080?key1";

   http::url::URLParser parser(expression);
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      BOOST_REQUIRE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Scheme), "tcp");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Host), "localhost.me");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Port), "8080");
   BOOST_REQUIRE(url->hasQuery());

   auto ii = url->query_begin();
   auto keyValue = URL::keyValue(ii);
   BOOST_REQUIRE_EQUAL(keyValue.first, "key1");
   BOOST_REQUIRE(keyValue.second.empty());

   BOOST_REQUIRE_EQUAL(url->encode(), expression);
}

BOOST_AUTO_TEST_CASE(URLParser_minimal_fragment)
{
   const bool expected[] = { true, false, false, true, false, false, true };
   const char* expression = "tcp://localhost.me#reference";

   http::url::URLParser parser(expression);
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      BOOST_REQUIRE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Scheme), "tcp");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Host), "localhost.me");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Fragment), "#reference");
   BOOST_REQUIRE(!url->hasQuery());

   BOOST_REQUIRE_EQUAL(url->encode(), expression);
}

BOOST_AUTO_TEST_CASE(URLParser_user)
{
   const bool expected[] = { true, true, false, true, false, false, true };
   const char* expression = "tcp://user@localhost.me#reference";

   http::url::URLParser parser(expression);
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      BOOST_REQUIRE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Scheme), "tcp");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::User), "user");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Host), "localhost.me");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Fragment), "#reference");
   BOOST_REQUIRE(!url->hasQuery());

   BOOST_REQUIRE_EQUAL(url->encode(), expression);
}

BOOST_AUTO_TEST_CASE(URLParser_pwd)
{
   const bool expected[] = { true, false, true, true, false, false, false };
   const char* expression = "tcp://:pwd@localhost.me";

   http::url::URLParser parser(expression);
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      BOOST_REQUIRE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Scheme), "tcp");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Password), "pwd");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Host), "localhost.me");
   BOOST_REQUIRE(!url->hasQuery());

   BOOST_REQUIRE_EQUAL(url->encode(), expression);
}

BOOST_AUTO_TEST_CASE(URLParser_user_pwd)
{
   const bool expected[] = { true, true, true, true, false, false, false };
   const char* expression = "sctp://one_guy:pwd@localhost.me";

   http::url::URLParser parser(expression);
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      BOOST_REQUIRE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Scheme), "sctp");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::User), "one_guy");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Password), "pwd");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Host), "localhost.me");
   BOOST_REQUIRE(!url->hasQuery());

   BOOST_REQUIRE_EQUAL(url->encode(), expression);
}

BOOST_AUTO_TEST_CASE(URLParser_empty_path)
{
   const bool expected[] = { true, false, false, true, false, true, false };
   const char* expression = "tcp://localhost.me/";

   http::url::URLParser parser(expression);
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      BOOST_REQUIRE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Scheme), "tcp");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Host), "localhost.me");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Path), "/");
   BOOST_REQUIRE(!url->hasQuery());

   BOOST_REQUIRE_EQUAL(url->encode(), expression);
}

BOOST_AUTO_TEST_CASE(URLParser_no_scheme)
{
   http::url::URLParser parser("localhost.me/");
   BOOST_REQUIRE_THROW(parser.build(), basis::RuntimeException);
}

BOOST_AUTO_TEST_CASE(URLParser_empty_scheme)
{
   http::url::URLParser parser(":localhost.me");
   BOOST_REQUIRE_THROW(parser.build(), basis::RuntimeException);
}

BOOST_AUTO_TEST_CASE(URLParser_no_host)
{
   http::url::URLParser parser("tcp://:8080");
   BOOST_REQUIRE_THROW(parser.build(), basis::RuntimeException);
}

BOOST_AUTO_TEST_CASE(URLParser_bad_query)
{
   http::url::URLParser parser("http://localhost.me?=zzz");
   BOOST_REQUIRE_THROW(parser.build(), basis::RuntimeException);
}

BOOST_AUTO_TEST_CASE(URLParser_no_numeric_port)
{
   http::url::URLParser parser("tcp://host.com:123a");
   BOOST_REQUIRE_THROW(parser.build(), basis::RuntimeException);
}
