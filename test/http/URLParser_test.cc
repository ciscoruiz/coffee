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
   http::url::URLParser parser("tcp://user:pwd@localhost.me:8080/path/resource?key1=value1&key2=value2#fragment");

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

   auto ii = url->keyvalue_begin();
   auto keyValue = URL::keyValue(ii);
   BOOST_REQUIRE_EQUAL(keyValue.first, "key1");
   BOOST_REQUIRE_EQUAL(keyValue.second, "value1");

   keyValue = URL::keyValue(++ ii);
   BOOST_REQUIRE_EQUAL(keyValue.first, "key2");
   BOOST_REQUIRE_EQUAL(keyValue.second, "value2");

   BOOST_REQUIRE(++ ii == url->keyvalue_end());
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
   BOOST_REQUIRE(!url->hasKeysValues());
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
   BOOST_REQUIRE(!url->hasKeysValues());
}

BOOST_AUTO_TEST_CASE(URLParser_minimal_port)
{
   const bool expected[] = { true, false, false, true, true, false, false };

   http::url::URLParser parser("tcp://localhost.me:8080");
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      BOOST_REQUIRE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Scheme), "tcp");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Host), "localhost.me");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Port), "8080");
   BOOST_REQUIRE(!url->hasKeysValues());
}

BOOST_AUTO_TEST_CASE(URLParser_minimal_path)
{
   const bool expected[] = { true, false, false, true, false, true, false };

   http::url::URLParser parser("tcp://localhost.me/path/resource");
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      BOOST_REQUIRE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Scheme), "tcp");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Host), "localhost.me");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Path), "/path/resource");
   BOOST_REQUIRE(!url->hasKeysValues());
}

BOOST_AUTO_TEST_CASE(URLParser_minimal_query)
{
   const bool expected[] = { true, false, false, true, true, false, false };

   http::url::URLParser parser("tcp://localhost.me:8080?key1=value1&key2=value2");
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      BOOST_REQUIRE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Scheme), "tcp");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Host), "localhost.me");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Port), "8080");
   BOOST_REQUIRE(url->hasKeysValues());

   auto ii = url->keyvalue_begin();
   auto keyValue = URL::keyValue(ii);
   BOOST_REQUIRE_EQUAL(keyValue.first, "key1");
   BOOST_REQUIRE_EQUAL(keyValue.second, "value1");
}

BOOST_AUTO_TEST_CASE(URLParser_minimal_fragment)
{
   const bool expected[] = { true, false, false, true, false, false, true };

   http::url::URLParser parser("tcp://localhost.me#reference");
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      BOOST_REQUIRE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Scheme), "tcp");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Host), "localhost.me");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Fragment), "#reference");
   BOOST_REQUIRE(!url->hasKeysValues());
}

BOOST_AUTO_TEST_CASE(URLParser_user)
{
   const bool expected[] = { true, true, false, true, false, false, true };

   http::url::URLParser parser("tcp://user@localhost.me#reference");
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      BOOST_REQUIRE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Scheme), "tcp");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::User), "user");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Host), "localhost.me");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Fragment), "#reference");
   BOOST_REQUIRE(!url->hasKeysValues());
}

BOOST_AUTO_TEST_CASE(URLParser_pwd)
{
   const bool expected[] = { true, false, true, true, false, false, false };

   http::url::URLParser parser("tcp://:pwd@localhost.me");
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      BOOST_REQUIRE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Scheme), "tcp");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Password), "pwd");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Host), "localhost.me");
   BOOST_REQUIRE(!url->hasKeysValues());
}

BOOST_AUTO_TEST_CASE(URLParser_user_pwd)
{
   const bool expected[] = { true, true, true, true, false, false, false };

   http::url::URLParser parser("tcp://one_guy:pwd@localhost.me");
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      BOOST_REQUIRE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Scheme), "tcp");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::User), "one_guy");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Password), "pwd");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Host), "localhost.me");
   BOOST_REQUIRE(!url->hasKeysValues());
}

BOOST_AUTO_TEST_CASE(URLParser_empty_path)
{
   const bool expected[] = { true, false, false, true, false, true, false };

   http::url::URLParser parser("tcp://localhost.me/");
   auto url = parser.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      BOOST_REQUIRE(url->hasComponent((ComponentName::_v) ii) == expected[ii]);
   }

   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Scheme), "tcp");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Host), "localhost.me");
   BOOST_REQUIRE_EQUAL(url->getComponent(ComponentName::Path), "/");
   BOOST_REQUIRE(!url->hasKeysValues());
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

BOOST_AUTO_TEST_CASE(URLParser_bad_query)
{
   http::url::URLParser parser("http://localhost.me?=zzz");
   BOOST_REQUIRE_THROW(parser.build(), basis::RuntimeException);
}

