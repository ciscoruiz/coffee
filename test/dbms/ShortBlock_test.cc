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

#include <iostream>
#include <time.h>

#include <boost/test/unit_test.hpp>

#include <coffee/dbms/datatype/ShortBlock.hpp>
#include <coffee/dbms/datatype/Integer.hpp>

using namespace coffee;
using namespace coffee::dbms;


BOOST_AUTO_TEST_CASE (shortblock_is_nulleable)
{
   datatype::ShortBlock column ("nulleable", 128, datatype::Constraint::CanBeNull);

   void* init = column.getBuffer();

   BOOST_REQUIRE_EQUAL (column.hasValue (), false);

   column.clear ();

   BOOST_REQUIRE_EQUAL (column.hasValue (), false);

   basis::DataBlock other;
   BOOST_REQUIRE_THROW (column.getValue (), basis::RuntimeException);

   other.assign ("hello world", 7);

   BOOST_REQUIRE_EQUAL (other.size (), 7);

   column.setValue (other);
   BOOST_REQUIRE_EQUAL (column.hasValue (), true);
   BOOST_REQUIRE_EQUAL (column.getSize(), 7);
   BOOST_REQUIRE_EQUAL (strncmp(column.getValue().data(), "hello w", 7), 0);

   column.clear ();
   BOOST_REQUIRE_EQUAL (column.hasValue (), false);
   BOOST_REQUIRE_EQUAL(column.getSize(), 0);

   BOOST_REQUIRE_EQUAL(init, column.getBuffer());
}

BOOST_AUTO_TEST_CASE (shortblock_is_not_nulleable)
{
   datatype::ShortBlock column ("not_nulleable", 4, datatype::Constraint::CanNotBeNull);

   void* init = column.getBuffer();

   BOOST_REQUIRE_EQUAL (column.hasValue (), true);

   basis::DataBlock other ("hello world", 7);

   BOOST_REQUIRE_THROW(column.setValue (other), basis::RuntimeException);

   other.assign ("hello", 4);
   column.setValue (other);
   BOOST_REQUIRE_EQUAL (column.hasValue (), true);
   BOOST_REQUIRE_EQUAL (column.getSize(), 4);
   BOOST_REQUIRE_EQUAL (strncmp(column.getValue().data(), "hello w", 4), 0);

   column.clear();
   BOOST_REQUIRE_EQUAL (column.hasValue(), true);
   BOOST_REQUIRE_EQUAL(column.getSize(), 0);

   BOOST_REQUIRE_EQUAL(init, column.getBuffer());

   other.assign ("size out of range");
   BOOST_REQUIRE_THROW (column.setValue (other), basis::RuntimeException);
}

BOOST_AUTO_TEST_CASE (shortblock_downcast)
{
   datatype::ShortBlock column ("not_nulleable", 4);

   datatype::Abstract& abs = column;

   auto other = coffee_datatype_downcast(datatype::ShortBlock, abs);

   datatype::Integer zzz ("zzz");

   BOOST_REQUIRE_THROW(coffee_datatype_downcast(datatype::ShortBlock, zzz), dbms::InvalidDataException);
}

BOOST_AUTO_TEST_CASE (shortblock_clone)
{
   datatype::ShortBlock column ("not_nulleable", 1025);

   const char* buffer = new char[1024];
   basis::DataBlock memory(buffer, 1024);

   column.setValue(memory);

   auto clone = coffee_datatype_downcast(datatype::ShortBlock, column.clone());

   BOOST_REQUIRE(clone->getValue() == column.getValue());
}
