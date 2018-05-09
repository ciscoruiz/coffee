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

#include <coffee/basis/RuntimeException.hpp>

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
   catch (basis::RuntimeException&) {
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
   catch (basis::RuntimeException& ex) {
      BOOST_REQUIRE ( strcmp ("this is goo", ex.what ()) == 0 );
      BOOST_REQUIRE(ex.asString ().find("RuntimeException_test.cc(32): int goo()] this is goo") != std::string::npos);
   }

   AAA aaa;

   try {
      aaa.member (10, 'z');
   }
   catch (basis::RuntimeException& ex) {
      BOOST_REQUIRE(ex.asString ().find("RuntimeException_test.cc(51): void AAA::member(int, char)] xx:10 zz:z") != std::string::npos);
      BOOST_REQUIRE(strcmp ("xx:10 zz:z", ex.what ()) == 0 );
   }

   try {
      AAA::member2 (11.11);
   }
   catch (basis::RuntimeException& ex) {
      BOOST_REQUIRE(ex.asString ().find("RuntimeException_test.cc(54): static void AAA::member2(float)] float: 11.110000") != std::string::npos);
      BOOST_REQUIRE(strcmp ("float: 11.110000", ex.what ()) == 0 );
   }
}

void hoo () {
   basis::RuntimeException ex ("this is hoo", COFFEE_FILE_LOCATION);
   ex.setErrorCode(100);
   throw ex;
}

BOOST_AUTO_TEST_CASE( RuntimeException_errorCode )
{
   try {
      hoo ();
   }
   catch (basis::RuntimeException& ex) {
      BOOST_REQUIRE(ex.asString ().find("RuntimeException_test.cc(88): void hoo()] ErrorCode: 100 | this is hoo") != std::string::npos);
      BOOST_REQUIRE(strcmp ("this is hoo", ex.what ()) == 0 );
   }
}

