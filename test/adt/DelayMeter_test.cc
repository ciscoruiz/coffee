// WEPA - Write Excellent Professional Applications
//
//(c) Copyright 2013 Francisco Ruiz Rayo
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#include <boost/test/unit_test.hpp>

#include <unistd.h>

#include <wepa/adt/DelayMeter.hpp>
#include <wepa/adt/Second.hpp>
#include <wepa/adt/Millisecond.hpp>
#include <wepa/adt/Microsecond.hpp>

using namespace wepa;

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
