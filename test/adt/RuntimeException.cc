// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2013 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/wepa
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

#include <wepa/adt/RuntimeException.hpp>

using namespace std;
using namespace wepa;

int goo () {
   WEPA_THROW_EXCEPTION ("this is goo");
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
      WEPA_THROW_EXCEPTION ("xx:" << xx << " zz:" << zz);
   }
   static void member2 (float zz) {
      WEPA_THROW_EXCEPTION ("float: " << zz);
   }
};

BOOST_AUTO_TEST_CASE( RuntimeException_asString )
{
   try {
      foo ();
   }
   catch (adt::RuntimeException& ex) {
      BOOST_REQUIRE ( strcmp ("this is goo", ex.what ()) == 0 );
      BOOST_REQUIRE_EQUAL (ex.asString (), "[test/adt/release/RuntimeException.cc(43): int goo()] this is goo");
   }

   AAA aaa;

   try {
      aaa.member (10, 'z');
   }
   catch (adt::RuntimeException& ex) {
      BOOST_REQUIRE_EQUAL (ex.asString (), "[test/adt/release/RuntimeException.cc(61): void AAA::member(int, char)] xx:10 zz:z");
      BOOST_REQUIRE ( strcmp ("xx:10 zz:z", ex.what ()) == 0 );
   }

   try {
      AAA::member2 (11.11);
   }
   catch (adt::RuntimeException& ex) {
      BOOST_REQUIRE_EQUAL (ex.asString (), "[test/adt/release/RuntimeException.cc(64): static void AAA::member2(float)] float: 11.110000");
      BOOST_REQUIRE ( strcmp ("float: 11.110000", ex.what ()) == 0 );
   }
}

void hoo () {
   adt::RuntimeException ex ("this is hoo", WEPA_FILE_LOCATION);
   ex.setErrorCode(100);
   throw ex;
}

BOOST_AUTO_TEST_CASE( RuntimeException_errorCode )
{
   try {
      hoo ();
   }
   catch (adt::RuntimeException& ex) {
      BOOST_REQUIRE_EQUAL (ex.asString (), "[test/adt/release/RuntimeException.cc(98): void hoo()] ErrorCode: 100 | this is hoo");
      BOOST_REQUIRE ( strcmp ("this is hoo", ex.what ()) == 0 );
   }
}

