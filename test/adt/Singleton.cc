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
#include <limits.h>

#include <iostream>

#include <boost/test/unit_test.hpp>

#include <wepa/adt/pattern/Singleton.hpp>

class TheClass : public wepa::adt::pattern::Singleton <TheClass> {
public:
   void setSome (const int some) { m_some = some; }
   int getSome () const throw () { return m_some; }

private:
   int m_some;

   TheClass () {;}

   friend class  wepa::adt::pattern::Singleton <TheClass>;
};

using namespace std;
using namespace wepa;

BOOST_AUTO_TEST_CASE( basic_use )
{ 
   {
      TheClass& clazz = TheClass::instantiate ();
      clazz.setSome (100);
   }
   {
      TheClass& clazz = TheClass::instantiate ();
      BOOST_REQUIRE_EQUAL (clazz.getSome (), 100);
   }
}

BOOST_AUTO_TEST_CASE( compare_pointer )
{ 
   const TheClass* p1 = TheClass::const_pointer ();
   const TheClass* p2 = TheClass::const_pointer ();
   BOOST_REQUIRE_EQUAL (p1, p2);
}
