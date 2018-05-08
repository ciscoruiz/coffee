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

#include <limits.h>

#include <iostream>

#include <boost/test/unit_test.hpp>

#include <coffee/basis/pattern/Singleton.hpp>

class TheClass : public coffee::basis::pattern::Singleton <TheClass> {
public:
   void setSome (const int some) { m_some = some; }
   int getSome () const noexcept { return m_some; }

private:
   int m_some;

   TheClass () : m_some(0) {;}

   friend class  coffee::basis::pattern::Singleton <TheClass>;
};

using namespace std;
using namespace coffee;

BOOST_AUTO_TEST_CASE( basic_use )
{ 
   {
      TheClass& clazz = TheClass::getInstance ();
      clazz.setSome (100);
   }
   {
      TheClass& clazz = TheClass::getInstance ();
      BOOST_REQUIRE_EQUAL (clazz.getSome (), 100);
   }
}

BOOST_AUTO_TEST_CASE( compare_pointer )
{ 
   const TheClass* p1 = TheClass::getConstPointer ();
   const TheClass* p2 = TheClass::getConstPointer ();
   BOOST_REQUIRE_EQUAL (p1, p2);
}
