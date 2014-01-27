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

#include <wepa/logger/Logger.hpp>
#include <wepa/logger/TtyWriter.hpp>

#include <wepa/app/Application.hpp>
#include <wepa/app/EngineIf.hpp>

using namespace std;
using namespace wepa;

class SmallestApplication : public app::Application {
public:
   SmallestApplication () : app::Application ("SmallerApplication", "This is the title", "1.0") {
      logger::Logger::initialize(new logger::TtyWriter);
   }

   void run () throw (adt::RuntimeException) {;}
};

class MyEngine : public app::EngineIf {
public:
   MyEngine (app::Application& application, const char* name) : EngineIf (application, name), m_initilized (0), m_stopped (0) {;}

   int getInitializedCounter () const noexcept { return m_initilized; }
   int getStoppedCounter () const noexcept { return m_stopped; }

   void setPredecessor (const char* predecessor) noexcept { this->addPredecessor(predecessor); }

private:
   int m_initilized;
   int m_stopped;

   void do_initialize () throw (adt::RuntimeException){ ++ m_initilized;}
   void do_stop () throw (adt::RuntimeException) { ++ m_stopped;}
};

BOOST_AUTO_TEST_CASE( smallest_application )
{ 
   SmallestApplication application;

   BOOST_REQUIRE_EQUAL (application.engine_find("00") == NULL, true);

   MyEngine myEngine00 (application, "00");

   application.start ();

   BOOST_REQUIRE_EQUAL (application.getPid(), getpid());

   BOOST_REQUIRE_EQUAL(&myEngine00.getApplication(), &application);
   BOOST_REQUIRE_EQUAL (application.engine_find("00"), &myEngine00);

   BOOST_REQUIRE_EQUAL (myEngine00.getInitializedCounter(), 1);
   BOOST_REQUIRE_EQUAL (myEngine00.getStoppedCounter(), 1);
}

BOOST_AUTO_TEST_CASE( status_application )
{
   SmallestApplication application;

   BOOST_REQUIRE_EQUAL(application.isStopped(), true);
   BOOST_REQUIRE_EQUAL(application.isRunning(), false);

   application.start ();

   BOOST_REQUIRE_EQUAL(application.isStopped(), true);
   BOOST_REQUIRE_EQUAL(application.isRunning(), false);
}


BOOST_AUTO_TEST_CASE( undefined_predecessor )
{
   SmallestApplication application;

   BOOST_REQUIRE_EQUAL (application.engine_find("00") == NULL, true);

   MyEngine myEngine00 (application, "00");

   myEngine00.setPredecessor("undefined");

   BOOST_REQUIRE_THROW(application.start (), adt::RuntimeException);;

   BOOST_REQUIRE_EQUAL (myEngine00.getInitializedCounter(), 0);
   BOOST_REQUIRE_EQUAL (myEngine00.getStoppedCounter(), 0);

   BOOST_REQUIRE_EQUAL(application.isStopped(), true);
   BOOST_REQUIRE_EQUAL(application.isRunning(), false);
}

BOOST_AUTO_TEST_CASE( interdependence_predecessor )
{
   SmallestApplication application;

   MyEngine myEngine00 (application, "00");
   MyEngine myEngine01 (application, "01");

   myEngine00.setPredecessor("01");
   myEngine01.setPredecessor("00");

   BOOST_REQUIRE_THROW(application.start (), adt::RuntimeException);;

   BOOST_REQUIRE_EQUAL (myEngine00.getInitializedCounter(), 0);
   BOOST_REQUIRE_EQUAL (myEngine00.getStoppedCounter(), 0);

   BOOST_REQUIRE_EQUAL(application.isStopped(), true);
   BOOST_REQUIRE_EQUAL(application.isRunning(), false);
}
