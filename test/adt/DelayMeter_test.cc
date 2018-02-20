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
