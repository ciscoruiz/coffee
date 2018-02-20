#include <boost/test/unit_test.hpp>

#include <unistd.h>

#include <coffee/adt/DelayMeter.hpp>
#include <coffee/adt/Second.hpp>
#include <coffee/adt/Millisecond.hpp>
#include <coffee/adt/Microsecond.hpp>

using namespace coffee;

BOOST_AUTO_TEST_CASE( delaymeter_seconds )
{
   adt::DelayMeter <adt::Second> meter;

   sleep(2);
   BOOST_REQUIRE_EQUAL(meter.getValue(), adt::Second(2));

   sleep(1);
   BOOST_REQUIRE_EQUAL(meter.getValue(), adt::Second(3));

   meter.reset();
   BOOST_REQUIRE_EQUAL(meter.getValue(), adt::Second(0));

   sleep(2);
   BOOST_REQUIRE_EQUAL(meter.getValue(), adt::Second(2));

   adt::DelayMeter <adt::Second> meter2(meter);
   BOOST_REQUIRE_EQUAL(meter.getValue(), meter2.getValue());

   meter2.reset();
   meter2 = meter;
   BOOST_REQUIRE_EQUAL(meter.getValue(), meter2.getValue());
}

BOOST_AUTO_TEST_CASE( delaymeter_milliseconds )
{
   adt::DelayMeter <adt::Millisecond> meter;

   usleep(2000);
   adt::Millisecond tt;

   tt = meter.getValue();
   BOOST_REQUIRE_GE(tt, adt::Millisecond(2));

   usleep(1100);
   tt = meter.getValue();
   BOOST_REQUIRE_GE(tt, adt::Millisecond(3));

   meter.reset();
   BOOST_REQUIRE_EQUAL(meter.getValue(), adt::Millisecond(0));

   usleep(2500);
   tt = meter.getValue();
   BOOST_REQUIRE_GE(tt, adt::Millisecond(2));
}
