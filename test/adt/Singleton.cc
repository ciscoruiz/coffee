#include <limits.h>

#include <iostream>

#include <boost/test/unit_test.hpp>

#include <coffee/adt/pattern/Singleton.hpp>

class TheClass : public coffee::adt::pattern::Singleton <TheClass> {
public:
   void setSome (const int some) { m_some = some; }
   int getSome () const noexcept { return m_some; }

private:
   int m_some;

   TheClass () {;}

   friend class  coffee::adt::pattern::Singleton <TheClass>;
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
