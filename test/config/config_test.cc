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
#define BOOST_TEST_MODULE WEPA_CONFIG_TEST

#include <boost/test/included/unit_test.hpp>

#include <iostream>

#include <limits.h>

#include <wepa/config/Release.h>
#include <wepa/config/defines.h>

using namespace std;
using namespace wepa;

BOOST_AUTO_TEST_CASE (release)
{
   string version = config::Release::getVersion();

   cout << version << endl;

   BOOST_REQUIRE (version.empty() == false);

   int debug = version.find("/D");
   int release = version.find("/O");

   #ifdef _DEBUG
      BOOST_REQUIRE (debug != string::npos);
      BOOST_REQUIRE (release == string::npos);
   #else
      BOOST_REQUIRE (debug == string::npos);
      BOOST_REQUIRE (release != string::npos);
   #endif
}

BOOST_AUTO_TEST_CASE (numbers)
{
   Integer64 ii64;

   ii64 = LLONG_MAX;
   BOOST_REQUIRE_EQUAL (ii64, LLONG_MAX);

   ii64 = LLONG_MIN;
   BOOST_REQUIRE_EQUAL (ii64, LLONG_MIN);

   Unsigned64 u64;
   u64 = ULLONG_MAX;
   BOOST_REQUIRE_EQUAL (u64, ULLONG_MAX);
}
