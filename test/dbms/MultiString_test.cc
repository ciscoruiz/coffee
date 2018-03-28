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

#include <coffee/dbms/datatype/MultiString.hpp>
#include <coffee/dbms/datatype/Integer.hpp>

using namespace coffee::dbms;

struct MultiStringFixture {
   static const char* values[];

   MultiStringFixture() : column("the-column", datatype::Constraint::CanNotBeNull) {
      int index = 0;
      while (values[index]) {
         column.addValue(values[index ++]);
      }
      BOOST_REQUIRE(column.size() == index);
   }

   datatype::MultiString column;
};

// static
const char* MultiStringFixture::values[] = { "1234", "3456", "1234", nullptr };

BOOST_AUTO_TEST_CASE(multistring_is_nulleable)
{
   datatype::MultiString column("nulleable", datatype::Constraint::CanBeNull);
   BOOST_REQUIRE(!column.hasValue());
   column.addValue("the value");
   BOOST_REQUIRE(column.hasValue());
}

BOOST_FIXTURE_TEST_CASE(multistring_iterator, MultiStringFixture)
{
   BOOST_REQUIRE(column.hasValue());

   int index = 0;
   for (auto ii = column.begin_value(), maxii = column.end_value(); ii != maxii; ++ ii) {
      BOOST_REQUIRE_EQUAL(datatype::MultiString::value(ii), values[index ++]);
   }
}

BOOST_FIXTURE_TEST_CASE(multistring_clone, MultiStringFixture)
{
   BOOST_REQUIRE(column.hasValue());

   auto clone = column.clone();

   BOOST_REQUIRE(clone->compare(column) == 0);
   BOOST_REQUIRE(clone->hash() == column.hash());
}

BOOST_FIXTURE_TEST_CASE(multistring_clear, MultiStringFixture)
{
   BOOST_REQUIRE(column.hasValue());
   column.clear();
   BOOST_REQUIRE(column.hasValue());
   BOOST_REQUIRE(column.size() == 0);
}

BOOST_AUTO_TEST_CASE(multistring_downcast)
{
   datatype::MultiString column("multistring_downcast");

   datatype::Abstract& abs = column;

   const datatype::MultiString& other = coffee_datatype_downcast(datatype::MultiString, abs);
   column.addValue("123");

   BOOST_REQUIRE(other == column);

   datatype::Integer zzz("zzz");

   BOOST_REQUIRE_THROW(coffee_datatype_downcast(datatype::MultiString, zzz), InvalidDataException);
}

BOOST_AUTO_TEST_CASE(multistring_instantiate) {
   auto data = datatype::MultiString::instantiate("nulleable");
   BOOST_REQUIRE(data->hasValue());

   data = datatype::MultiString::instantiate("not-nulleable", datatype::Constraint::CanBeNull);
   BOOST_REQUIRE(!data->hasValue());
}
