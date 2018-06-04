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

#include <coffee/http/url/URLBuilder.hpp>
#include <coffee/http/url/URL.hpp>

using namespace coffee;

using coffee::http::url::URL;
using http::url::ComponentName;

TEST(HttpUrlBuilder, set_all_components)
{
   http::url::URLBuilder builder;

   const char* values[] = { "http", "user", "password", "host.com.zzz", "1234", "/path/resource", "fragment"};

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      builder.setCompoment((ComponentName::_v) ii, values[ii]);
   }

   builder.addKeyValue("key1", "value1").addKeyValue("key2", "value2");

   auto url = builder.build();

   for (int ii = 0; ii < ComponentName::Fragment; ++ ii) {
      ASSERT_TRUE(url->hasComponent((ComponentName::_v) ii));
      ASSERT_EQ(values[ii], url->getComponent((ComponentName::_v) ii));
   }

   http::url::KeyValue kv = URL::keyValue(url->query_begin());
   ASSERT_TRUE(kv.first == "key1");
   ASSERT_TRUE(kv.second == "value1");
}

TEST(HttpUrlBuilder, no_component_defined)
{
   http::url::URLBuilder builder;

   builder.setCompoment(ComponentName::Scheme, "tcp");
   builder.setCompoment(ComponentName::Host, "localhost");

   auto url = builder.build();

   ASSERT_TRUE(url->hasComponent(ComponentName::Scheme));
   ASSERT_TRUE(url->hasComponent(ComponentName::Host));
   ASSERT_TRUE(!url->hasComponent(ComponentName::User));
   ASSERT_TRUE(!url->hasComponent(ComponentName::Password));
   ASSERT_THROW(url->getComponent(ComponentName::Path), basis::RuntimeException);
}

TEST(HttpUrlBuilder, without_mandatories)
{
   {
      http::url::URLBuilder builder;
      builder.setCompoment(ComponentName::Scheme, "tcp");
      ASSERT_THROW(builder.build(), basis::RuntimeException);

   }
   {
      http::url::URLBuilder builder;
      builder.setCompoment(ComponentName::Host, "localhost");
      ASSERT_THROW(builder.build(), basis::RuntimeException);
   }
}

TEST(HttpUrlBuilder, repeat_component)
{
   http::url::URLBuilder builder;
   builder.setCompoment(ComponentName::Port, "123");

   ASSERT_THROW(builder.setCompoment(ComponentName::Port, "444"), basis::RuntimeException);
}
