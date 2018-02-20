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

