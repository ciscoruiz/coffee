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

#include <coffee/dbms/datatype/LongBlock.hpp>
#include <coffee/dbms/datatype/Integer.hpp>

using namespace coffee;
using namespace coffee::dbms;


BOOST_AUTO_TEST_CASE (longblock_is_nulleable)
{
   datatype::LongBlock column ("nulleable", datatype::Constraint::CanBeNull);

   BOOST_REQUIRE_EQUAL (column.hasValue (), false);

   column.clear ();

   BOOST_REQUIRE_EQUAL (column.hasValue (), false);

   adt::DataBlock other;
   BOOST_REQUIRE_THROW (column.getValue (), adt::RuntimeException);

   other.assign ("hello world", 7);

   column.setValue (other);
   BOOST_REQUIRE_EQUAL (column.hasValue (), true);
   BOOST_REQUIRE_EQUAL (column.getSize(), 7);
   BOOST_REQUIRE_EQUAL (strncmp(column.getValue().data(), "hello w", 7), 0);

   column.clear ();
   BOOST_REQUIRE_EQUAL (column.hasValue (), false);
   BOOST_REQUIRE_EQUAL(column.getSize(), 0);
}

BOOST_AUTO_TEST_CASE (longblock_is_not_nulleable)
{
   datatype::LongBlock column ("not_nulleable", datatype::Constraint::CanNotBeNull);

   BOOST_REQUIRE_EQUAL (column.hasValue (), true);

   adt::DataBlock other ("hello world", 7);

   column.setValue (other);
   BOOST_REQUIRE_EQUAL (column.hasValue (), true);
   BOOST_REQUIRE_EQUAL (column.getSize(), 7);
   BOOST_REQUIRE_EQUAL (strncmp(column.getValue().data(), "hello w", 7), 0);

   column.clear();
   BOOST_REQUIRE_EQUAL (column.hasValue(), true);
   BOOST_REQUIRE_EQUAL(column.getSize(), 0);
}

BOOST_AUTO_TEST_CASE (longblock_downcast)
{
   datatype::LongBlock column ("not_nulleable");

   datatype::Abstract& abs = column;

   auto other = coffee_datatype_downcast(datatype::LongBlock, abs);

   datatype::Integer zzz ("zzz");

   BOOST_REQUIRE_THROW(coffee_datatype_downcast(datatype::LongBlock, zzz), dbms::InvalidDataException);
}

BOOST_AUTO_TEST_CASE (longblock_clone)
{
   datatype::LongBlock column ("not_nulleable");

   const char* buffer = new char[1024];
   adt::DataBlock memory(buffer, 1024);

   column.setValue(memory);

   auto clone = coffee_datatype_downcast(datatype::LongBlock, column.clone());

   BOOST_REQUIRE(clone->getValue() == column.getValue());
}

BOOST_AUTO_TEST_CASE (longblock_clone_innerscope)
{
   std::shared_ptr<datatype::LongBlock> clone;

   {
      datatype::LongBlock column ("not_nulleable");
      adt::DataBlock memory("1234", 4);
      column.setValue(memory);
      clone = coffee_datatype_downcast(datatype::LongBlock, column.clone());
   }

   BOOST_REQUIRE(clone->getValue().size() == 4);
   BOOST_REQUIRE(memcmp(clone->getValue().data(), "1234", 4) == 0);
}


