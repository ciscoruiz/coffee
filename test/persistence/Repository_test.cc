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

#include <boost/test/unit_test.hpp>

#include <map>
#include <functional>
#include <mutex>
#include <thread>

#include <wepa/adt/RuntimeException.hpp>

#include <wepa/logger/Logger.hpp>
#include <wepa/logger/TraceMethod.hpp>
#include <wepa/logger/TtyWriter.hpp>

#include <wepa/persistence/Repository.hpp>
#include <wepa/persistence/Storage.hpp>

#include <wepa/xml/Node.hpp>
#include <wepa/xml/Compiler.hpp>

using namespace wepa;

BOOST_AUTO_TEST_CASE (persistence_define_structure)
{
   persistence::Repository repository ("persistence_define_structure");

   persistence::Storage* ii = repository.createStorage(0, "the 0", persistence::Storage::AccessMode::ReadOnly, NULL);

   BOOST_REQUIRE_NE (ii, (void*) 0);

   persistence::Storage& find = repository.findStorage(0);

   BOOST_REQUIRE_EQUAL (ii, &find);

   BOOST_REQUIRE_THROW(repository.createStorage(0, "already used", persistence::Storage::AccessMode::ReadOnly, NULL), adt::RuntimeException);

   persistence::Storage* jj = repository.createStorage(1, "the 1", persistence::Storage::AccessMode::ReadOnly, NULL);

   BOOST_REQUIRE_NE (ii, jj);

   BOOST_REQUIRE_THROW(repository.createStorage(333, "invalid access", persistence::Storage::AccessMode::None, NULL), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE (persistence_repository_as)
{
   persistence::Repository repository ("persistence_define_structure");

   repository.createStorage(0, "the 0", persistence::Storage::AccessMode::ReadOnly, NULL);
   repository.createStorage(1, "the 1", persistence::Storage::AccessMode::ReadWrite, NULL);
   repository.createStorage(2, "the 2", persistence::Storage::AccessMode::ReadAlways, NULL);

   adt::StreamString zz = repository.asString ();

   BOOST_REQUIRE_EQUAL (zz, "persistence.Repository { adt::NamedObject { Name: persistence_define_structure } | N-Size=3 }");

   xml::Node myNode ("root");

   xml::Node& info = repository.asXML(myNode);

   BOOST_REQUIRE_EQUAL (info.children_size(), 3);

   xml::Node& zero = info.childAt(0);
   BOOST_REQUIRE_EQUAL (zero.lookupAttribute("Name").getValue(), "the 0");
   BOOST_REQUIRE_EQUAL (zero.lookupAttribute("Mode").getValue(), "ReadOnly");

   xml::Node& one = info.childAt(1);
   BOOST_REQUIRE_EQUAL (one.lookupAttribute("Name").getValue(), "the 1");
   BOOST_REQUIRE_EQUAL (one.lookupAttribute("Mode").getValue(), "ReadWrite");

   xml::Node& two = info.childAt(2);
   BOOST_REQUIRE_EQUAL (two.lookupAttribute("Name").getValue(), "the 2");
   BOOST_REQUIRE_EQUAL (two.lookupAttribute("Mode").getValue(), "ReadAlways");
}
