
#include <boost/test/unit_test.hpp>

#include <iostream>

#include <coffee/adt/Second.hpp>
#include <coffee/adt/Millisecond.hpp>
#include <coffee/adt/Microsecond.hpp>

using namespace std;
using namespace coffee;

BOOST_AUTO_TEST_CASE( TimeClasses_second )
{
   adt::Second guide = adt::Second::getTime();
   adt::Millisecond ms (guide);
   adt::Microsecond us (guide);

   BOOST_REQUIRE (guide == ms);
   BOOST_REQUIRE (guide == us);

   adt::Second other;

   other = ms;
   BOOST_REQUIRE_EQUAL (guide, other);

   other = us;
   BOOST_REQUIRE_EQUAL (guide, other);
}

BOOST_AUTO_TEST_CASE( TimeClasses_local_second )
{
   adt::Second guide = adt::Second::getLocalTime();
   adt::Millisecond ms (guide);
   adt::Microsecond us (guide);

   BOOST_REQUIRE (guide == ms);
   BOOST_REQUIRE (guide == us);

   adt::Second other;

   other = ms;
   BOOST_REQUIRE_EQUAL (guide, other);

   other = us;
   BOOST_REQUIRE_EQUAL (guide, other);
}

BOOST_AUTO_TEST_CASE( TimeClasses_millisecond )
{
   adt::Millisecond guide = adt::Millisecond::getTime();
   adt::Second sec (guide);
   adt::Microsecond us (guide);

   BOOST_REQUIRE_EQUAL (guide, sec);
   BOOST_REQUIRE_EQUAL (guide, us);

   adt::Millisecond other;

   other = sec;
   BOOST_REQUIRE (other <= guide);

   other = us;
   BOOST_REQUIRE_EQUAL (guide, other);
}

BOOST_AUTO_TEST_CASE( TimeClasses_microsecond )
{
   adt::Microsecond guide = adt::Microsecond::getTime();
   adt::Second sec (guide);
   adt::Millisecond ms (guide);

   BOOST_REQUIRE_EQUAL (guide, sec);
   BOOST_REQUIRE_EQUAL (guide, ms);

   adt::Microsecond other;

   other = sec;
   BOOST_REQUIRE (other <= guide);

   other = ms;
   BOOST_REQUIRE (other <= guide);
}

BOOST_AUTO_TEST_CASE( TimeClasses_second_from_string )
{
   adt::Second guide(LONG_MAX);

   BOOST_REQUIRE_EQUAL (guide.getValue(), LONG_MAX);

   std::string str = guide.asString();

   adt::Second other = adt::Second::fromString(str);

   BOOST_REQUIRE_EQUAL (other, guide);

   guide = 10;
   BOOST_REQUIRE_EQUAL ( guide.asString(), "10 sec");
}

BOOST_AUTO_TEST_CASE( TimeClasses_millisecond_from_string )
{
   adt::Millisecond guide(LLONG_MAX);

   BOOST_REQUIRE_EQUAL (guide.getValue(), LLONG_MAX);

   std::string str = guide.asString();

   cout << "str: " << str << endl;

   adt::Millisecond other = adt::Millisecond::fromString(str);

   BOOST_REQUIRE_EQUAL (other, guide);

   guide = 10;
   BOOST_REQUIRE_EQUAL ( guide.asString(), "10 ms");
}

BOOST_AUTO_TEST_CASE( TimeClasses_millisecond_compare )
{
   adt::Millisecond start(adt::Millisecond::getTime());
   usleep(5000);
   adt::Millisecond after(adt::Millisecond::getTime());
   BOOST_REQUIRE(start < after);
   BOOST_REQUIRE(after > start);
   BOOST_REQUIRE(start != after);
}

BOOST_AUTO_TEST_CASE( TimeClasses_microsecond_from_string )
{
   adt::Microsecond guide(LLONG_MAX);

   BOOST_REQUIRE_EQUAL (guide.getValue(), LLONG_MAX);

   std::string str = guide.asString();

   cout << "str: " << str << endl;

   adt::Microsecond other = adt::Microsecond::fromString(str);

   BOOST_REQUIRE_EQUAL (other, guide);

   guide = 10;
   BOOST_REQUIRE_EQUAL ( guide.asString(), "10 us");
}

BOOST_AUTO_TEST_CASE( TimeClasses_microsecond_compare )
{
   adt::Microsecond start(adt::Microsecond::getTime());
   usleep(5000);
   adt::Microsecond after(adt::Microsecond::getTime());
   BOOST_REQUIRE(start < after);
   BOOST_REQUIRE(after > start);
   BOOST_REQUIRE(start != after);
}

BOOST_AUTO_TEST_CASE( TimeClasses_excep_from_string )
{
   BOOST_CHECK_THROW (adt::Second::fromString("10 xx"), adt::RuntimeException);
   BOOST_CHECK_THROW (adt::Millisecond::fromString("10 xx"), adt::RuntimeException);
   BOOST_CHECK_THROW (adt::Microsecond::fromString("10 xx"), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE( TimeClasses_as_date_time )
{
   adt::Second sec (INT_MAX);
   std::string str = sec.asDateTime();
   BOOST_REQUIRE(str.find("19/01/2038 ") != std::string::npos);
   BOOST_REQUIRE(str.find(":14:07") != std::string::npos);
}

BOOST_AUTO_TEST_CASE( TimeClasses_bad_time )
{
	adt::Second sec (LONG_MAX);
	BOOST_CHECK_THROW (sec.asDateTime(), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE( TimeClasses_fromstring)
{
   const char* format = "%Y-%m-%d %H:%M:%S";

   adt::Second now = adt::Second::getTime();

   for (int ii = 0; ii < 100; ++ ii) {
      adt::Second newTime = now + (rand() % 1000);
      const std::string dateString = newTime.asDateTime(format);
      BOOST_REQUIRE_EQUAL(adt::Second::fromString(dateString, format), newTime);
   }
}

BOOST_AUTO_TEST_CASE( TimeClasses_fromstring_bad_format)
{
   const char* format = "%Y-%m-%d %H:%M:%S";

   adt::Second now = adt::Second::getTime();

   const std::string dateString = now.asDateTime(format);

   BOOST_REQUIRE_THROW (adt::Second::fromString(dateString, "invalid-format"), adt::RuntimeException);
}

