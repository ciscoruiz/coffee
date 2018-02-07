// COFFEE - COmpany eFFEEctive Platform
//
// (c) Copyright 2018 Francisco Ruiz Rayo
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#include <boost/test/unit_test.hpp>

#include <coffee/adt/RuntimeException.hpp>

using namespace std;
using namespace coffee;

int goo () {
   COFFEE_THROW_EXCEPTION ("this is goo");
   return 0;
}

int foo () {
   try {
      goo ();
   }
   catch (adt::RuntimeException&) {
      throw;
   }

   return 0;
}


class AAA {
public:
   void member (int xx, char zz) {
      COFFEE_THROW_EXCEPTION ("xx:" << xx << " zz:" << zz);
   }
   static void member2 (float zz) {
      COFFEE_THROW_EXCEPTION ("float: " << zz);
   }
};

BOOST_AUTO_TEST_CASE( RuntimeException_asString )
{
   try {
      foo ();
   }
   catch (adt::RuntimeException& ex) {
      BOOST_REQUIRE ( strcmp ("this is goo", ex.what ()) == 0 );
      BOOST_REQUIRE(ex.asString ().find("RuntimeException_test.cc(43): int goo()] this is goo") != std::string::npos);
   }

   AAA aaa;

   try {
      aaa.member (10, 'z');
   }
   catch (adt::RuntimeException& ex) {
      BOOST_REQUIRE(ex.asString ().find("RuntimeException_test.cc(62): void AAA::member(int, char)] xx:10 zz:z") != std::string::npos);
      BOOST_REQUIRE(strcmp ("xx:10 zz:z", ex.what ()) == 0 );
   }

   try {
      AAA::member2 (11.11);
   }
   catch (adt::RuntimeException& ex) {
      BOOST_REQUIRE(ex.asString ().find("RuntimeException_test.cc(65): static void AAA::member2(float)] float: 11.110000") != std::string::npos);
      BOOST_REQUIRE(strcmp ("float: 11.110000", ex.what ()) == 0 );
   }
}

void hoo () {
   adt::RuntimeException ex ("this is hoo", COFFEE_FILE_LOCATION);
   ex.setErrorCode(100);
   throw ex;
}

BOOST_AUTO_TEST_CASE( RuntimeException_errorCode )
{
   try {
      hoo ();
   }
   catch (adt::RuntimeException& ex) {
      BOOST_REQUIRE(ex.asString ().find("RuntimeException_test.cc(99): void hoo()] ErrorCode: 100 | this is hoo") != std::string::npos);
      BOOST_REQUIRE(strcmp ("this is hoo", ex.what ()) == 0 );
   }
}

