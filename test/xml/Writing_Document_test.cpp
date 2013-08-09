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
#include <boost/filesystem.hpp>

#include <wepa/adt/DataBlock.hpp>
#include <wepa/adt/Second.hpp>

#include <wepa/xml/Document.hpp>
#include <wepa/xml/Node.hpp>
#include <wepa/xml/Attribute.hpp>

using namespace wepa;
using namespace wepa::xml;

BOOST_AUTO_TEST_CASE (create_nodes)
{
   std::auto_ptr <xml::Node> root (new xml::Node ("the_root"));

   xml::Node& level00 = root->createChild("level0-0");
   level00.createChild ("level1-0");
   level00.createChild ("level1-1");
   level00.createChild ("level1-2");
   level00.createChild ("level1-3").createChild ("2-0");
   BOOST_REQUIRE_EQUAL(level00.children_size(), 4);

   root->createChild ("level0-1");
   root->createChild ("level0-2");

   BOOST_REQUIRE_EQUAL(root->children_size(), 3);

   BOOST_REQUIRE_EQUAL (root->lookupChild("level0-0").lookupChild("level1-0").children_size(), 0);
   BOOST_REQUIRE_EQUAL (root->lookupChild("level0-0").lookupChild("level1-3").children_size(), 1);
}

BOOST_AUTO_TEST_CASE (create_text)
{
   std::auto_ptr <xml::Node> root (new xml::Node ("the_root"));

   xml::Node& level00 = root->createChild("level0-0");
   level00.createText ("0-0");
   level00.createChild ("level1-3").createText ("2-0");
   BOOST_REQUIRE_EQUAL(level00.children_size(), 1);

   BOOST_REQUIRE_EQUAL (level00.getText(), "0-0");
   BOOST_REQUIRE_EQUAL (level00.lookupChild("level1-3").getText (), "2-0");
}

BOOST_AUTO_TEST_CASE (create_attribute)
{
   std::auto_ptr <xml::Node> root (new xml::Node ("the_root"));

   root->createAttribute ("attr_char", "This is the char");
   root->createAttribute ("attr_int", 1024);
   root->createAttribute ("attr_second", adt::Second (59));

   BOOST_REQUIRE_EQUAL (root->attributes_size(), 3);

   BOOST_REQUIRE_EQUAL (root->lookupAttribute("attr_char").getValue (), "This is the char");
   BOOST_REQUIRE_EQUAL (root->lookupAttribute("attr_int").getValue (), "1024");
   BOOST_REQUIRE_EQUAL (root->lookupAttribute("attr_second").getValue (), "59");
}

BOOST_AUTO_TEST_CASE (change_attribute)
{
   std::auto_ptr <xml::Node> root (new xml::Node ("the_root"));

   root->createAttribute ("attr_char", "This is the char");

   BOOST_REQUIRE_EQUAL (root->attributes_size(), 1);

   BOOST_REQUIRE_EQUAL (root->lookupAttribute("attr_char").getValue (), "This is the char");

   {
      std::string innerScope = "zzz";
      root->searchAttribute("attr_char")->setValue (innerScope);
   }

   int ii;

   BOOST_REQUIRE_EQUAL (root->lookupAttribute("attr_char").getValue (), "zzz");
}
