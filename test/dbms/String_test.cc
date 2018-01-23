// COFFEE - COmpany eFFEEctive Platform
//
//(c) Copyright 2018 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/coffee
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#include <iostream>
#include <time.h>

#include <boost/test/unit_test.hpp>

#include <coffee/dbms/datatype/String.hpp>
#include <coffee/dbms/datatype/Integer.hpp>

using namespace coffee;
using namespace coffee::dbms;

BOOST_AUTO_TEST_CASE(string_is_nulleable)
{
   datatype::String column("nulleable", 16, datatype::Constraint::CanBeNull);

   void* init = column.getBuffer();

   BOOST_REQUIRE_EQUAL(column.hasValue(), false);

   column.clear();

   BOOST_REQUIRE_EQUAL(column.hasValue(), false);
   BOOST_REQUIRE_THROW(column.getValue(), adt::RuntimeException);
   BOOST_REQUIRE_EQUAL(column.getSize(), 0);

   column.setValue("hello world");
   BOOST_REQUIRE_EQUAL(column.hasValue(), true);
   BOOST_REQUIRE_EQUAL(column.getSize(), coffee_strlen("hello world"));
   BOOST_REQUIRE_EQUAL(strncmp(column.getValue(), "hello world", coffee_strlen("hello world")), 0);

   column.clear();
   BOOST_REQUIRE_EQUAL(column.hasValue(), false);
   BOOST_REQUIRE_EQUAL(column.getSize(), 0);

   column.setValue("zzz");
   BOOST_REQUIRE_EQUAL(column.hasValue(), true);

   column.setValue(NULL);
   BOOST_REQUIRE_EQUAL(column.hasValue(), false);

   BOOST_REQUIRE_EQUAL(init, column.getBuffer());

   BOOST_REQUIRE_THROW(column.setValue("size out of range"), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(string_is_not_nulleable)
{
   datatype::String column("not_nulleable", 4, datatype::Constraint::CanNotBeNull);

   void* init = column.getBuffer();

   BOOST_REQUIRE_EQUAL(column.hasValue(), true);

   column.setValue("bye");
   BOOST_REQUIRE_EQUAL(column.hasValue(), true);
   BOOST_REQUIRE_EQUAL(column.getSize(), coffee_strlen("bye"));
   BOOST_REQUIRE_EQUAL(strncmp(column.getValue(), "bye", coffee_strlen("bye")), 0);

   column.clear();
   BOOST_REQUIRE_EQUAL(column.hasValue(), true);
   BOOST_REQUIRE_EQUAL(column.getSize(), 0);

   BOOST_REQUIRE_EQUAL(init, column.getBuffer());

   BOOST_REQUIRE_THROW(column.setValue("size out of range"), adt::RuntimeException);
   BOOST_REQUIRE_THROW(column.setValue(NULL), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(string_downcast)
{
   datatype::String column("string_downcast", 4);

   datatype::Abstract& abs = column;

   const datatype::String& other = coffee_datatype_downcast(datatype::String, abs);
   column.setValue("123");

   BOOST_REQUIRE_EQUAL(other == column, true);

   datatype::Integer zzz("zzz");

   BOOST_REQUIRE_THROW(coffee_datatype_downcast(datatype::String, zzz), dbms::InvalidDataException);
}

BOOST_AUTO_TEST_CASE(string_clone)
{
   datatype::String cannotBeNull("cannotBeNull", 16, datatype::Constraint::CanNotBeNull);
   datatype::String canBeNull("canBeNull", 16, datatype::Constraint::CanBeNull);

   BOOST_REQUIRE_EQUAL(cannotBeNull.hasValue(), true);
   BOOST_REQUIRE_EQUAL(canBeNull.hasValue(), false);

   auto notnull(cannotBeNull.clone());
   auto null(canBeNull.clone());

   BOOST_REQUIRE_EQUAL(notnull->hasValue(), true);
   BOOST_REQUIRE_EQUAL(null->hasValue(), false);

   BOOST_REQUIRE_EQUAL(notnull->compare(cannotBeNull), 0);

   cannotBeNull.setValue("abcd");

   BOOST_REQUIRE_EQUAL(cannotBeNull.getValue(), "abcd");

   notnull = cannotBeNull.clone();
   BOOST_REQUIRE_EQUAL(notnull->hasValue(), true);
   BOOST_REQUIRE_EQUAL(notnull->compare(cannotBeNull), 0);

   canBeNull.setValue("xzy");
   null = canBeNull.clone();
   BOOST_REQUIRE_EQUAL(null->hasValue(), true);
   BOOST_REQUIRE_EQUAL(null->compare(canBeNull), 0);
   BOOST_REQUIRE_GT(null->compare(cannotBeNull), 0);

   BOOST_REQUIRE_LT(notnull->compare(canBeNull), 0);
}
