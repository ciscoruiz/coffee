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
#include <boost/test/unit_test.hpp>

#include <coffee/persistence/ClassBuilder.hpp>
#include <coffee/persistence/PrimaryKeyBuilder.hpp>
#include <coffee/persistence/PrimaryKey.hpp>

#include <coffee/dbms/datatype/Integer.hpp>

using namespace coffee;

BOOST_AUTO_TEST_CASE(persistence_classbuilder_twice_pks)
{
   std::shared_ptr<persistence::PrimaryKey> primaryKey;

   {
      persistence::PrimaryKeyBuilder pkBuilder;
      pkBuilder.add(std::make_shared<dbms::datatype::Integer>("id"));
      primaryKey = pkBuilder.build();
   }

   persistence::ClassBuilder classBuilder("customer");
   BOOST_REQUIRE_NO_THROW(classBuilder.set(primaryKey));

   std::shared_ptr<persistence::PrimaryKey> badPrimaryKey;
   {
      persistence::PrimaryKeyBuilder pkBuilder;
      pkBuilder.add(std::make_shared<dbms::datatype::Integer>("some-other-stuff"));
      badPrimaryKey = pkBuilder.build();
   }

   BOOST_REQUIRE_THROW(classBuilder.set(badPrimaryKey), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(persistence_classbuilder_without_pk)
{
   persistence::ClassBuilder classBuilder("customer");

   classBuilder.add(std::make_shared<dbms::datatype::Integer>("id")).add(std::make_shared<dbms::datatype::Integer>("age"));
   BOOST_REQUIRE_THROW(classBuilder.build(), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(persistence_classbuilder_without_fields)
{
   persistence::ClassBuilder classBuilder("customer");

   persistence::PrimaryKeyBuilder pkBuilder;
   pkBuilder.add(std::make_shared<dbms::datatype::Integer>("id"));
   auto primaryKey = pkBuilder.build();

   BOOST_REQUIRE_NO_THROW(classBuilder.set(primaryKey));
   BOOST_REQUIRE_THROW(classBuilder.build(), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(persistence_classbuilder_dual_member)
{
   static const char* NameInPkAndMember = "id";
   persistence::ClassBuilder classBuilder("customer");

   persistence::PrimaryKeyBuilder pkBuilder;
   pkBuilder.add(std::make_shared<dbms::datatype::Integer>(NameInPkAndMember));
   auto primaryKey = pkBuilder.build();

   BOOST_REQUIRE_NO_THROW(classBuilder.set(primaryKey));
   classBuilder.add(std::make_shared<dbms::datatype::Integer>("size")).add(std::make_shared<dbms::datatype::Integer>(NameInPkAndMember));

   BOOST_REQUIRE_THROW(classBuilder.build(), adt::RuntimeException);
}
